#ifndef _HASH_H
#define _HASH_H

#include "util.h"

struct util_hash {
	unsigned int objects;
	unsigned int collisions;
	unsigned int bins;
	unsigned int (*function)(void *key, size_t len);
	struct util_hash_bin {
		unsigned int hash;
		unsigned int objects;
		struct util_hash_obj {
			void *key;
			void *object;
			size_t key_len;
		} *obj;
	} *bin;
};

struct util_hash *util_hash_new(unsigned int bins, const char *hash_function);
unsigned int util_hash_insert(struct util_hash *h, void *key, size_t len, void *object);
void *util_hash_find(struct util_hash *h, void *key, size_t len);
void util_hash_delete(struct util_hash *h);
void util_hash_delete_all(struct util_hash *h);

#endif /* _HASH_H */
