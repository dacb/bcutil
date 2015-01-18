#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "util.h"
#include "fasta.h"
#include "fastq.h"
#include "error.h"
#include "memory.h"
#include "oligo_freq.h"
#include "gff.h"
#include "gc_scan.h"
#include "nsplit.h"

int test_read_fasta(int argc, char *argv[]);
int test_read_fastq(int argc, char *argv[]);
int test_oligo_freq(int argc, char *argv[]);
int test_sndup(int argc, char *argv[]);
int test_read_gff(int argc, char *argv[]);
int test_aacomp(int argc, char *argv[]);
int test_gc_scan(int argc, char *argv[]);
int test_nsplit(int argc, char *argv[]);
int test_pairwise_identity(int argc, char *argv[]);
int test_get_upstream(int argc, char *argv[]);

struct unit_tests {
	char	*name;
	int	(*function)(int argc, char *argv[]);
} unit_tests[] = { \
	{ "fasta_read",		&test_read_fasta }, \
	{ "fastq_read",		&test_read_fastq }, \
	{ "oligo_freq",		&test_oligo_freq }, \
	{ "sndup",		&test_sndup }, \
	{ "gff_read",		&test_read_gff }, \
	{ "aacomp",		&test_aacomp }, \
	{ "gc_scan",		&test_gc_scan }, \
	{ "nsplit",		&test_nsplit }, \
	{ "pairwise_identity",	&test_pairwise_identity }, \
	{ "get_upstream", &test_get_upstream }, \
	{ NULL,			NULL } /* end marker */ \
};

int main(int argc, char *argv[]) {
	int i;

	if (argc < 2)
		fatal_error("usage: %s <unit> [<unit test options>]\n", argv[0]);

	for (i = 0; unit_tests[i].name != NULL; ++i)
		if (strcmp(unit_tests[i].name, argv[1]) == 0)
			exit(unit_tests[i].function(argc, argv));

	warning("%s: supported unit tests are:\n", argv[0]);
	for (i = 0; unit_tests[i].name != NULL; ++i)
		warning("\t%s\n", unit_tests[i].name);
	exit(EXIT_FAILURE);
}

int test_get_upstream(int argc, char *argv[]) {
	int i;
	gff_t *gf;
	struct util_ptr_list *seqs, *outseqs;
	int offset, max, minlen;

	if (argc != 7)
		fatal_error("usage: %s %s <gff3 filename> <fasta filename (may be gzipped)> <start relative to gene start, e.g. -2 to get ATG> <upstream end, e.g. 150> <min sequence length, e.g. 8>\n", argv[0], argv[1]);

	gf = gffopenreadclose(argv[2]);
	if (!gf) {
		warning("%s: an error occurred reading the GFF file '%s'\n", argv[0], argv[2]);
		return EXIT_FAILURE;
	}
	printf("%s: found %d gene features in GFF file '%s'\n", argv[0], gf->genes, argv[2]);
	seqs = fasta_read(argv[3]);
	printf("%s: found %d sequences in fasta file '%s'\n", argv[0], seqs->ps, argv[3]);

	offset = atoi(argv[4]);
	max = atoi(argv[5]);
	if (max - offset >= MAX_FASTA_LINE - 1)
		fatal_error("%s: internal error, upstream requested region larger than MAX_FASTA_LINE\n", argv[0]);
	minlen = atoi(argv[6]);
	if (minlen < 0)
		fatal_error("%s: minimum length is less than 1! (%d)\n", argv[0], minlen);

	printf("locus_tag\tstrand\tstart\tend\tlength\tidxstart\tidxend\tuplength\n");
	int idxstart, idxend;
	for (i = 0; i < gf->genes; ++i) {
		gene_t *gi = &gf->gene_features[i];
		char str[MAX_FASTA_LINE] = { '\0' };
		struct fasta *gseq = NULL;
		int j, k, gseqlen, slen = 0;
		for (j = 0; j < seqs->ps; ++j) {
			struct fasta *seq = (struct fasta *)seqs->p[j];
			if (strcmp(seq->uid, gi->locus) == 0) {
				gseq = seq;
				break;
			}
		}
		if (gseq == NULL) {
			fatal_error("%s %s: unable to focus sequence in fasta by name '%s'\n", argv[0], argv[1], gi->locus);
		}
		gseqlen = strlen(gseq->seq);
		idxstart = (gi->strand == PLUS ? gi->start - offset : gi->end + offset);
		idxend = idxstart;
		for (k = offset; k < max; ++k) {
			int idx = (gi->strand == PLUS ? gi->start - k : gi->end + k);
			for (j = 0; j < gf->genes; ++j) {
				if (j == i) continue;
				gene_t *gj = &gf->gene_features[j];
				if (gi->strand == PLUS && gi->strand == gj->strand && idx >= gj->start && idx < gj->end)
					break;
				else if (gi->strand == MINUS && gi->strand == gj->strand && idx > gj->start && idx <= gj->end)
					break;
			}
			// no overlap with other ranges, copy this base
			if (j == gf->genes) {
				if (idx - 1 < 0 || idx - 1 >= gseqlen)
					break;
				char c = gseq->seq[idx - 1];
				c = (gi->strand == PLUS ? c : (c == 'A' ? 'T' : (c == 'T' ? 'A' : (c == 'G' ? 'C' : 'G'))));
				str[k - offset] = c;
				str[k - offset + 1] = '\0';
				idxend = idx;
				++slen;
			} else {
				break;
			}
		}
		if (slen >= minlen) {
			strrev(str);
			printf("%s\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\n", gi->locus_tag, str, (gi->strand == MINUS ? "-" : "+"), gi->start, gi->end, (gi->end - gi->start) - 1, idxstart, idxend, slen);
		}
	}

	return EXIT_SUCCESS;

}

int test_pairwise_identity(int argc, char *argv[]) {
	int i, j, k;
	int seqlen, len, ident;
	struct util_ptr_list *seqs;

	if (argc != 3)
		fatal_error("usage: %s %s <fasta filename (may be gzipped)>\n", argv[0], argv[1]);

	seqs = fasta_read(argv[2]);

	printf("seq_i\tseq_j\t%% identity\talignment length\tidentical\n");
	for (i = 0; i < seqs->ps; ++i) {
		struct fasta *seq_i = (struct fasta *)seqs->p[i];
		seqlen = strlen(seq_i->seq);
		for (j = i + 1; j < seqs->ps; ++j) {
			struct fasta *seq_j = (struct fasta *)seqs->p[j];
			int seqlen_j = strlen(seq_j->seq);
			ident = 0;
			len = 0;
			if (seqlen != seqlen_j) {
				fatal_error("%s %s: sequence lengths differ for %s and %s.  Is this an aligned FASTA?\n", argv[0], argv[1], seq_i->uid, seq_j->uid);
			}
			for(k = 0; k < seqlen; ++k) {
				if (seq_i->seq[k] != '-' || seq_j->seq[k] != '-') {
					if (seq_i->seq[k] == seq_j->seq[k])
						++ident;
					++len;
				}
			}
			printf("%s\t%s\t%.2f\t%d\t%d\n", seq_i->uid, seq_j->uid, ((float)ident / (float)len) * 100., len, ident);
		}
	}
	return EXIT_SUCCESS;
}

int test_read_fasta(int argc, char *argv[]) {
	int i, j;
	int seqlen, max_seqlen = 0, contigs_1k = 0;
	long int sum_seqlen = 0, sum_seqlen_1k = 0, gc = 0, n = 0, called = 0;
	double mean_seqlen, n50;
	struct util_ptr_list *seqs;
	struct util_qsort_blob *seqlens;
	struct util_int_list *seqlens_n50;

	if (argc != 3)
		fatal_error("usage: %s %s <fasta filename (may be gzipped)>\n", argv[0], argv[1]);

	seqs = fasta_read(argv[2]);

	seqlens = (struct util_qsort_blob *)new(sizeof(struct util_qsort_blob) * seqs->ps);
	memset(seqlens, 0, sizeof(struct util_qsort_blob) * seqs->ps);

	for (i = 0; i < seqs->ps; ++i) {
		struct fasta *seq = (struct fasta *)seqs->p[i];
		seqlen = strlen(seq->seq);
		sum_seqlen += seqlen;
		if (seqlen > max_seqlen)
			max_seqlen = seqlen;
		if (seqlen >= 1000) {
			sum_seqlen_1k += seqlen;
			contigs_1k ++;
		}
		for (j = 0; j < seqlen; ++j)
			if (seq->seq[j] == 'G' || seq->seq[j] == 'C' || seq->seq[j] == 'g' || seq->seq[j] == 'c')
				gc++;
			else if (seq->seq[j] == 'N' || seq->seq[j] == 'n')
				n++;
			else called++;
		seqlens[i].integer = seqlen;
/*
		printf("sequence %d:\n\theader\t|%s|\n\tuid\t|%s|\n\tdesc\t|%s|\n\tseq\t|%s|\n", i + 1, seq->header, seq->uid, seq->desc, seq->seq);
*/
	}
	called += gc;
	mean_seqlen = (double)((double)sum_seqlen / (double)seqs->ps);

	qsort(seqlens, seqs->ps, sizeof(struct util_qsort_blob), util_qsort_blob_by_integer);
	seqlens_n50 = util_int_list_new();
	for (i = 0; i < seqs->ps; ++i) {
		for (j = 0; j < seqlens[i].integer; ++j) {
			util_int_list_add_int(seqlens_n50, seqlens[i].integer);
		}
	}
	i = seqlens_n50->is;
	if (i % 2 == 1) {
		n50 = ((double)seqlens_n50->i[i/2] + (double)seqlens_n50->i[(i/2)+1]) / 2.0;
	} else {
		n50 = seqlens_n50->i[i/2];
	}

	printf("max sequence length is %d\n", max_seqlen);
	printf("mean sequence length is %g\n", mean_seqlen);
	printf("number of contigs > 1k is %d\n", contigs_1k);
	printf("bases in all sequences is %ld\n", sum_seqlen);
	printf("bases in sequences > 1k is %ld\n", sum_seqlen_1k);
	printf("N50 is %.2f\n", n50);
	printf("GC %% = %.2f\n", ((double)gc / (double)called) * 100.);
	printf("uncalled bases (N) = %ld or %.f %%\n", n, ((double)n / (double)sum_seqlen) * 100.);
	
	return EXIT_SUCCESS;
}

int test_oligo_freq(int argc, char *argv[]) {
	int i;
	struct util_ptr_list *seqs;
	struct oligo_frequency_container *ofc;
	long score;

	if (argc != 4 || atoi(argv[3]) <= 0)
		fatal_error("usage: %s %s <fastq filename (may be gzipped)> <complexity score oligo word size, e.g. 3>\n", argv[0], argv[1]);
	seqs = fastq_read(argv[2], 0);

	ofc = oligo_frequency_init(atoi(argv[3]));
	for (i = 0; i < seqs->ps; ++i) {
		struct fastq *seq = (struct fastq *)seqs->p[i];
		score = oligo_frequency_score(ofc, seq->seq);
		if (score < 0)
			continue;
		printf("%s\t%s\t%ld\n", seq->uid, seq->seq, score);
	}
	oligo_frequency_destroy(ofc);

	return EXIT_SUCCESS;
}

int test_read_fastq(int argc, char *argv[]) {
	int i, j, len, max_len = 0;
	struct util_ptr_list *seqs;
	enum quality_scale scale;
	long bases = 0;
	double avglength;
	double *qmeans, *qcounts, *qstddev, *qerrors, *qestddev;
	double *seqcnt;
	int ti;
	float *q;

	if (argc != 4)
		fatal_error("usage: %s %s <fastq filename (may be gzipped)> <quality scale: 'sanger', 'solexa', 'illumina'>\n", argv[0], argv[1]);
	scale = fastq_text_to_quality_scale(argv[3]);

	seqs = fastq_read(argv[2], 1);

	for (i = 0; i < seqs->ps; ++i) {
		struct fastq *seq = (struct fastq *)seqs->p[i];
		len = strlen(seq->seq);
		if (len > max_len)
			max_len = len;
		bases += len;
	}
	avglength = (double)bases / (double)seqs->ps;
	printf("max sequence length is %d\n", max_len);
	printf("average sequence length is %.2f\n", avglength);
	printf("total number of bases is %ld\n", bases);

	qmeans = (double *)new(max_len * sizeof(double));
	memset(qmeans, 0, max_len * sizeof(double));
	qcounts = (double *)new(max_len * sizeof(double));
	memset(qcounts, 0, max_len * sizeof(double));
	qstddev = (double *)new(max_len * sizeof(double));
	memset(qstddev, 0, max_len * sizeof(double));
	qerrors = (double *)new(max_len * sizeof(double));
	memset(qerrors, 0, max_len * sizeof(double));
	qestddev = (double *)new(max_len * sizeof(double));
	memset(qestddev, 0, max_len * sizeof(double));
	q = (float *)new(max_len * sizeof(float));

	seqcnt = (double *)new(max_len * 6 * sizeof(double));
	memset(seqcnt, 0, max_len * 6 * sizeof(double));

	for (i = 0; i < seqs->ps; ++i) {
		struct fastq *seq = (struct fastq *)seqs->p[i];
		len = strlen(seq->qual);
		fastq_fill_qualities_from_letters(seq->qual, scale, q);
		for (j = 0; j < len; ++j)
			qmeans[j] += (double)q[j];
		for (j = 0; j < len; ++j)
			qerrors[j] += error_from_phred_quality(q[j]);
		for (j = 0; j < len; ++j)
			qcounts[j]++;
		for (j = 0; j < len; ++j) {
			switch (seq->seq[j]) {
				case 'A':
				case 'a':
					ti = 0;
					break;
				case 'T':
				case 't':
					ti = 1;
					break;
				case 'C':
				case 'c':
					ti = 2;
					break;
				case 'G':
				case 'g':
					ti = 3;
					break;
				case 'N':
				case 'n':
					ti = 4;
					break;
				default:
					ti = 5;
					break;
			};
			seqcnt[(j * 6) + ti]++;
		}
	}

	for (i = 0; i < max_len; ++i)
		qmeans[i] /= qcounts[i];
	for (i = 0; i < max_len; ++i)
		qerrors[i] /= qcounts[i];

	for (i = 0; i < seqs->ps; ++i) {
		struct fastq *seq = (struct fastq *)seqs->p[i];
		len = strlen(seq->seq);
		fastq_fill_qualities_from_letters(seq->qual, scale, q);
		for (j = 0; j < len; ++j)
			qstddev[j] += pow((double)q[j] - qmeans[j], 2);
		for (j = 0; j < len; ++j)
			qestddev[j] += pow(error_from_phred_quality(q[j]) - qerrors[j], 2);
	}

	for (i = 0; i < max_len; ++i)
		qstddev[i] *= 1.0 / qcounts[i];
	for (i = 0; i < max_len; ++i)
		qstddev[i] = sqrt(qstddev[i]);

	for (i = 0; i < max_len; ++i)
		qestddev[i] *= 1.0 / qcounts[i];
	for (i = 0; i < max_len; ++i)
		qestddev[i] = sqrt(qestddev[i]);

	for (i = 0; i < max_len; ++i)
		for (j = 0; j < 6; ++j)
			seqcnt[(i * 6) + j] /= (double)seqs->ps;

	printf("readpos\tsamples\tmeanQ\t\tstddevQ\t\tE(meanQ)\tE(stddevQ)\tmeanE\t\tstddevE\tfracA\tfracT\tfracC\tfracG\tfracN\tfracX\n");
	for (i = 0; i < max_len; ++i)
		printf("%d\t%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n", i, (unsigned int)qcounts[i], qmeans[i], qstddev[i], error_from_phred_quality(qmeans[i]), error_from_phred_quality(qstddev[i]), qerrors[i], qestddev[i], seqcnt[(i * 6) + 0], seqcnt[(i * 6) + 1], seqcnt[(i * 6) + 2], seqcnt[(i * 6) + 3], seqcnt[(i * 6) + 4], seqcnt[(i * 6) + 5]);

	delete(qmeans);
	delete(qcounts);
	delete(q);

	
	return EXIT_SUCCESS;
}

int test_sndup(int argc, char *argv[]) {
	char *string;

	if (argc != 3)
		fatal_error("usage: %s %s <string>\n", argv[0], argv[1]);

	string = sndup("the string '%s' is %d characters long", argv[2], strlen(argv[2]));
	printf("%s\n", string);
	delete(string);
	
	return EXIT_SUCCESS;
}

int test_read_gff(int argc, char *argv[]) {
	gff_t *gf;

	if (argc != 3)
		fatal_error("usage: %s %s <gff filename>\n", argv[0], argv[1]);

	gf = gffopenreadclose(argv[2]);
	if (!gf) {
		warning("%s: an error occurred reading the GFF file '%s'\n", argv[0], argv[2]);
		return EXIT_FAILURE;
	}
	printf("%s: found %d gene features in GFF file '%s'\n", argv[0], gf->genes, argv[2]);

	return EXIT_SUCCESS;
}

int test_aacomp(int argc, char *argv[]) {
	int i, j, sum;
	struct util_ptr_list *seqs;
	int *aahist;

	if (argc != 3)
		fatal_error("usage: %s %s <fasta filename (may be gzipped)>\n", argv[0], argv[1]);

	seqs = fasta_read(argv[2]);

	aahist = (int *)new(sizeof(int) * 26);
	memset(aahist, 0, sizeof(int) * 26);

	for (i = 0; i < seqs->ps; ++i) {
		struct fasta *seq = (struct fasta *)seqs->p[i];
		int seqlen = strlen(seq->seq);
		for (j = 0; j < seqlen; ++j) {
			char c = toupper(seq->seq[j]);
			aahist[c - 'A']++;
		}
	}
	for (i = 0, sum = 0; i < 26; ++i) {
		sum += aahist[i];
	}
	for (i = 0; i < 26; ++i) {
		printf("%c\t%d\t%.4f\n", i + 'A', aahist[i], (double)aahist[i] / (double)sum);
	}
	
	return EXIT_SUCCESS;
}

int test_gc_scan(int argc, char *argv[]) {
	struct util_ptr_list *seqs;
	struct gc_scan *gcs;
	int window_size;

	if (argc != 4)
		fatal_error("usage: %s %s <window size in bases> <fasta filename (may be gzipped)>\n", argv[0], argv[1]);

	window_size = atoi(argv[2]);
	seqs = fasta_read(argv[3]);

	gcs = gc_scan_init(window_size, seqs);

	gc_scan_report(gcs);

	gc_scan_destroy(gcs);

	return EXIT_SUCCESS;
}

#define MAXSEQLINE 60

int test_nsplit(int argc, char *argv[]) {
	struct util_ptr_list *seqs;
	struct nsplit *nsp;
	int max_n_before_split;
	int i;
	FILE *fp;
	char line[MAXSEQLINE + 1];

	if (argc != 5)
		fatal_error("usage: %s %s <number of N bases in a row to split a contig> <fasta input filename (may be gzipped)> <fasta output filename>\n", argv[0], argv[1]);

	max_n_before_split = atoi(argv[2]);
	seqs = fasta_read(argv[3]);

	nsp = nsplit_init(max_n_before_split, seqs);
	fprintf(stderr, "input sequences split into %d contigs when %d or more Ns appeared in a row\n", nsp->out->ps, max_n_before_split);

	if ((fp = fopen(argv[4], "w")) == NULL)	{
		fatal_error("%s %s: unable to open file '%s' for writing\n", argv[0], argv[1], argv[4]);
	}
	for (i = 0; i < nsp->out->ps; ++i) {
		struct fasta *f = (struct fasta *)nsp->out->p[i];
		int j, k, slen = strlen(f->seq);
		fprintf(fp, "%s", f->header);
		for (j = 0, k = 0; j < slen; ++j) {
			line[k++] = f->seq[j];
			if (k == MAXSEQLINE) {
				line[k] = '\0';
				fprintf(fp, "%s\n", line);
				k = 0;
			}
		}
		line[k] = '\0';
		fprintf(fp, "%s\n", line);
	}
	fclose(fp);

	nsplit_destroy(nsp);

	return EXIT_SUCCESS;
}
