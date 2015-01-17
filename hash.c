#include <string.h>
#include "hash.h"
#include "memory.h"
#include "error.h"

unsigned int hash_rotating(void *key, size_t len) {
	unsigned char *c = key;
	unsigned int h = 0;
	size_t i;
	for (i = 0; i < len; ++i)
		h = (h << 4) ^ (h >> 28) ^ c[i];
	return h;
}

struct util_hash_functions {
	char *name;
	unsigned int (*function)(void *key, size_t len);
};

struct util_hash_functions hash_functions[] = { \
	{ "rotating",	&hash_rotating }, \
	{ NULL,		NULL } \
};

static unsigned int (*hash_function_by_name(const char *function_name))(void *key, size_t len) {
	int i;
	for (i = 0; hash_functions[i].name != NULL; ++i) {
		if (strcmp(hash_functions[i].name, function_name) == 0)
			return hash_functions[i].function;
	}
	return NULL;
}

struct util_hash *util_hash_new(unsigned int bins, const char *function_name) {
	char *func_text = "util_hash_new";
	struct util_hash *h;
	unsigned int (*function)(void *key, size_t len);
	unsigned int i;

	function = hash_function_by_name(function_name);
	if (function == NULL)
		fatal_error("%s: unable to find hash function with name '%s'\n", func_text, function_name);

	h = (struct util_hash *)new(sizeof(struct util_hash));
	h->function = function;
	h->bins = bins;
	h->bin = (struct util_hash_bin *)new(h->bins * sizeof(struct util_hash_bin));
	memset(h->bin, 0, h->bins * sizeof(struct util_hash_bin));
	for (i = 0; i < h->bins; ++i)
		h->bin[i].hash = i;

	return h;
}

unsigned int util_hash_insert(struct util_hash *h, void *key, size_t len, void *object) {
	unsigned int bi = h->function(key, len) % h->bins;
	struct util_hash_bin *bin = &h->bin[bi];
	unsigned int oi = bin->objects;
	bin->objects++;
	bin->obj = (struct util_hash_obj *)renew(bin->obj, bin->objects * sizeof(struct util_hash_obj));
	bin->obj[oi].key = key;
	bin->obj[oi].object = object;
	bin->obj[oi].key_len = len;
	if (bin->objects > 1)
		h->collisions++;
	h->objects++;

	return bi;
}

void *util_hash_find(struct util_hash *h, void *key, size_t len) {
	unsigned int bi = h->function(key, len) % h->bins;
	struct util_hash_bin *bin = &h->bin[bi];
	unsigned int oi = bin->objects;
	unsigned int i, j;
	for (i = 0; i < oi; ++i) {
		char *a, *b;
		if (len != bin->obj[i].key_len)
			continue;
		a = (char *)bin->obj[i].key;
		b = (char *)key;
		for (j = 0; j < len; ++j)
			if (a[j] != b[j])
				break;
		if (j == len)
			return bin->obj[i].object;
	}

	return NULL;
}

void util_hash_delete(struct util_hash *hash) {
	int i;

	for (i = 0; i < hash->bins; ++i) {
		delete(hash->bin[i].obj);
	}
	delete(hash->bin);
	delete(hash);
}

/* note this only works when an object just consists of an allocated key pointer and a single allocated struct pointer */
void util_hash_delete_all(struct util_hash *hash) {
	int i, j;

	for (i = 0; i < hash->bins; ++i) {
		for (j = 0; j < hash->bin[i].objects; ++j) {
			delete(hash->bin[i].obj[j].key);
			delete(hash->bin[i].obj[j].object);
		}
	}
	util_hash_delete(hash);
}
