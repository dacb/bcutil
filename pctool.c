#include <string.h>

#include "pctool.h"
#include "error.h"
#include "memory.h"

#undef PCTOOL_DEBUG
#define PCTOOL_DEBUG

#ifdef PCTOOL_DEBUG
#include <stdio.h>
#endif

struct pctool *pctool_init(int buffer_size) {
	/* allocate and init the basic data structure */
	struct pctool *pctool = (struct pctool *)new(sizeof(struct pctool));
	memset(pctool, 0, sizeof(struct pctool));
	pctool->buffer_size = buffer_size;
	pctool->buffer = (void *)new(pctool->buffer_size * sizeof(void *));
	memset(pctool->buffer, 0, pctool->buffer_size * sizeof(void *));

	/* intialize mutual exclusion and condition variables */
	pthread_mutex_init(&pctool->mutex, NULL);
	pthread_cond_init(&pctool->more, NULL);
	pthread_cond_init(&pctool->less, NULL);

	/* mark the thing as initialized and pass it back */
	pctool->init = 1;
	return pctool;
}

void pctool_put(struct pctool *pctool, void *data) {
	pthread_mutex_lock(&pctool->mutex);

	while (pctool->occupied >= pctool->buffer_size)
		pthread_cond_wait(&pctool->less, &pctool->mutex);

	//assert(pctool->occupied < buffer_size);

	pctool->buffer[pctool->nextin++] = data;
	pctool->nextin %= pctool->buffer_size;
	pctool->occupied++;

    /* now: either pctool->occupied < buffer_size and pctool->nextin is the index
       of the next empty slot in the buffer, or
       pctool->occupied == buffer_size and pctool->nextin is the index of the
       next (occupied) slot that will be emptied by a consumer
       (such as pctool->nextin == pctool->nextout) */

	pthread_cond_signal(&pctool->more);
	pthread_mutex_unlock(&pctool->mutex);
}

void *pctool_get(struct pctool *pctool) {
	void *data;

	pthread_mutex_lock(&pctool->mutex);
	
	while(pctool->occupied <= 0)
		pthread_cond_wait(&pctool->more, &pctool->mutex);

	//assert(pctool->occupied > 0);

	data = pctool->buffer[pctool->nextout++];
	pctool->nextout %= pctool->buffer_size;
	pctool->occupied--;

    /* now: either pctool->occupied > 0 and pctool->nextout is the index
       of the next occupied slot in the buffer, or
       pctool->occupied == 0 and pctool->nextout is the index of the next
       (empty) slot that will be filled by a producer (such as
       pctool->nextout == pctool->nextin) */

	pthread_cond_signal(&pctool->less);
	pthread_mutex_unlock(&pctool->mutex);

	return data;
}

void pctool_destroy(struct pctool *pctool) {
	pctool->init = 0;
	delete(pctool->buffer);

	pthread_mutex_destroy(&pctool->mutex);
	pthread_cond_destroy(&pctool->more);
	pthread_cond_destroy(&pctool->less);

	delete(pctool);
}
