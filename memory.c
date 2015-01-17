#include <unistd.h>
#include <stdlib.h>
#include "memory.h"
#include "error.h"

void *new(size_t size) {
	void	*data;

	if (size == 0) {
		warning("new: requested zero bytes of memory, continuing...\n");
		return NULL;
	}
	data = (void *)malloc(size);
	if (data == NULL)
		fatal_error("new: could not allocate %d byte block\n", size);
	return data;
}

void delete(void *data) {
	if (data != NULL) {
		free(data);
		data = NULL;
	}
}

void *renew(void *data, size_t size) {
	void	*new_data;

	if (size == 0) {
		warning("renew: requested zero bytes of memory, continuing...\n");
		return NULL;
	}
	new_data = (void *)realloc(data, size);
	if (new_data == NULL)
		fatal_error("renew: could not reallocate %d byte block (0x%x)\n", size, data);
	return new_data;
}
