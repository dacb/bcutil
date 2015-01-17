#include <string.h>
#include <math.h>

#include "util.h"
#include "hash.h"
#include "error.h"
#include "memory.h"
#include "oligo_freq.h"

static char na[] = NUCLEOTIDE_ALPHABET;

struct oligo_frequency_container *oligo_frequency_init(int oligo_length) {
	struct oligo_frequency_container *ofc;

	ofc = (struct oligo_frequency_container *)new(sizeof(struct oligo_frequency_container));
	memset(ofc, 0, sizeof(struct oligo_frequency_container));

	ofc->len = oligo_length;
	ofc->unique = pow(NUCLEOTIDE_ALPHABET_SIZE, ofc->len);
	ofc->oligos = util_hash_new(ofc->unique, "rotating");
	
	return ofc;
}

void oligo_frequency_reset(struct oligo_frequency_container *ofc) {
	int i, j;

	for (i = 0; i < ofc->oligos->bins; ++i) {
		for (j = 0; j < ofc->oligos->bin[i].objects; ++j) {
			struct oligo_frequency *of = (struct oligo_frequency *)ofc->oligos->bin[i].obj[j].object;
			of->freq = 0;
		}
	}
}

long oligo_frequency_score(struct oligo_frequency_container *ofc, char *sequence) {
	int len, i, j, k;
	long score = 0;
	char *oligo = (char *)new(sizeof(char) * (ofc->len + 1));
	oligo[ofc->len] = '\0';

	len = strlen(sequence) - ofc->len;

	oligo_frequency_reset(ofc);

	for (i = 0; i <= len; ++i) {
		struct oligo_frequency *of;
		for (j = 0; j < ofc->len; ++j) {
			oligo[j] = sequence[i + j];
			for (k = 0; k < NUCLEOTIDE_ALPHABET_SIZE; ++k)
				if (oligo[j] == na[k])
					break;
			if (k == NUCLEOTIDE_ALPHABET_SIZE) {
				score = -1;
				goto error;
			}
		}
		of = (struct oligo_frequency *)util_hash_find(ofc->oligos, (void *)oligo, ofc->len * sizeof(char));
		if (of) {
			of->freq++;
		} else {
			of = (struct oligo_frequency *)new(sizeof(struct oligo_frequency));
			of->seq = strdup(oligo);
			of->freq = 1;
			util_hash_insert(ofc->oligos, (void *)of->seq, ofc->len * sizeof(char), of);
		}
	}

	for (i = 0; i <= len; ++i) {
		struct oligo_frequency *of;
		for (j = 0; j < ofc->len; ++j)
			oligo[j] = sequence[i + j];
		of = (struct oligo_frequency *)util_hash_find(ofc->oligos, (void *)oligo, ofc->len * sizeof(char));
		score += (of->freq * of->freq);
	}

error:
	delete(oligo);
	return score;
}

void oligo_frequency_destroy(struct oligo_frequency_container *ofc) {
	util_hash_delete_all(ofc->oligos);
	delete(ofc);
}
