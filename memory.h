#ifndef _MEMORY_H
#define _MEMORY_H

void *new(size_t size);
void delete(void *data);
void *renew(void *data, size_t size);

#endif /* _MEMORY_H */
