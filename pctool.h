#ifndef _PCTOOL_H
#define _PCTOOL_H

#include <pthread.h>
#include <semaphore.h>

struct pctool {
	int init;		/* structure is initialized when = 1, = 0 otherwise */
	int buffer_size;	/* size of the void pointer buffer */
	void **buffer;	/* void pointer array for storing data */
    int occupied;
    int nextin;
    int nextout;
    pthread_mutex_t mutex;
    pthread_cond_t more;
    pthread_cond_t less;
};

struct pctool *pctool_init(int buffer_size);
void pctool_put(struct pctool *pctool, void *data);
void *pctool_get(struct pctool *pctool);
void pctool_destroy(struct pctool *pctool);

#endif /* _PCTOOL_H */
