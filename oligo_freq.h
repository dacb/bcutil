#ifndef _OLIGO_FREQ_H

#define _OLIGO_FREQ_H

struct oligo_frequency {
	char *seq;
	unsigned int freq;
};

struct oligo_frequency_container {
	int len;
	int unique;
	struct util_hash *oligos;
};

#define NUCLEOTIDE_ALPHABET_SIZE	4		/* ATCG */
#define NUCLEOTIDE_ALPHABET		{ 'A', 'T', 'C', 'G' }

struct oligo_frequency_container *oligo_frequency_init(int oligo_length);
void oligo_frequency_reset(struct oligo_frequency_container *ofc);
long oligo_frequency_score(struct oligo_frequency_container *ofc, char *sequence);
void oligo_frequency_destroy(struct oligo_frequency_container *ofc);

#endif /* _OLIGO_FREQ_H */
