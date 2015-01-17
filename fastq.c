#include <string.h>
#include <zlib.h>
#include <math.h>

#include "util.h"
#include "error.h"
#include "memory.h"
#include "fastq.h"

void fastq_delete(struct fastq *f) {
	if (f->uid)
		delete(f->uid);
	if (f->seq)
		delete(f->seq);
	if (f->qual)
		delete(f->qual);
	delete(f);
}

struct fastq *fastq_new(const char *seqname) {
	struct fastq *f = (struct fastq *)new(sizeof(struct fastq));
	f->uid = strdup(seqname);
	f->seq = NULL;
	f->qual = NULL;
	return f;
}

struct util_ptr_list *fastq_read(const char *filename, int warnings_are_fatal) {
	char *func_text = "fastq_read";
	gzFile *gz;
	struct util_ptr_list *seqs;
	char line[MAX_FASTQ_LINE];
	int in_seq = 0, line_no = 0, found_seq = 0, found_seqname = 0;
	struct fastq *seq;
	void (* output)(const char *fmt, ...) = (warnings_are_fatal == 1 ? &fatal_error : &warning);

	if ((gz = gzopen(filename, "rb")) == NULL) {
		fatal_error("%s: unable to open file '%s'\n", func_text, filename);
	}

	seqs = util_ptr_list_new();

	while (gzgets(gz, line, MAX_FASTQ_LINE) != NULL) {
		char c;
		line_no++;
		if (line[0] == '\n')
			continue;
		else if (line[0] == '@') {
			if (in_seq && seq != NULL) {
				/* @ is a valid symbol for quality so some quality lines may begin with it,
				this goto handles that special case */
				if (found_seq && seq->qual == NULL)
					goto QUALITY_SPECIAL_CASE_FOR_LINE_BEGINING_WITH_AT_SYMBOL;
				if (!found_seq) {
					output("%s: no sequence string found for sequence %s near line %d\n", func_text, seq->uid, line_no - 1);
					fastq_delete(seq);
					seq = NULL;
				} else if (seq->qual == NULL) {
					output("%s: no quality string found for sequence %s near line %d\n", func_text, seq->uid, line_no - 1);
					fastq_delete(seq);
					seq = NULL;
				} else
					if (seq != NULL)
						util_ptr_list_add_ptr(seqs, seq);
			}
			c = line[strlen(line) - 1];
			while(c == '\n' || c == '\r' || c == '\t') {
				line[strlen(line) - 1] = '\0';
				c = line[strlen(line) - 1];
			}
			seq = fastq_new(&line[1]);
			seq->seq = (char *)new(sizeof(char));
			seq->seq[0] = '\0';
			in_seq = 1;
			found_seq = 0;
			found_seqname = 0;
#if 0
			int i;
			/* enforce uniqueness of uid */
			for (i = 0; i < seqs->ps; ++i) {
				struct fastq *f = seqs->p[i];
				if (strcmp(f->uid, seq->uid) == 0) {
					warning("%s: ignored entry with duplicate unique sequence name (%s) on line %d\n", func_text, f->uid, line_no);
					fastq_delete(seq);
					in_seq = 0;
					break;
				}
			}
#endif
		} else {
			if (in_seq) {
				if (!found_seq) {
					seq->seq = strdup(line);
					seq->seq[strlen(line) - 1] = '\0';	/* toss new line */
					found_seq = 1;
				} else if (line[0] == '+' && !found_seqname) {
					/* ignore optional seqname repeat */
					found_seqname = 1;
				} else { /* quality */
/* this symbol below is used to handle a special case above, search for symbol name */
QUALITY_SPECIAL_CASE_FOR_LINE_BEGINING_WITH_AT_SYMBOL:
					/* process a quality line */
					seq->qual = strdup(line);
					seq->qual[strlen(line) - 1] = '\0';	/* toss new line */
					if (strlen(seq->qual) != strlen(seq->seq)) {
						output("%s: for sequence '%s' there is a quality string and sequence string length mismatch on line no. %d\n", func_text, seq->uid, line_no);
						fastq_delete(seq);
						seq = NULL;
					}
				}
			} else
				warning("%s: ignored contents of line no. %d\n", func_text, line_no);
		}
	}
	if (in_seq && seq != NULL)
		util_ptr_list_add_ptr(seqs, seq);

	gzclose(gz);

	printf("fastq_read: found %d sequences in '%s'\n", seqs->ps, filename);

	return seqs;
}

enum quality_scale fastq_text_to_quality_scale(const char *name) {
	char *func_text = "fastq_text_to_quality_scale";
	if (strcmp("sanger", name) == 0)
		return SANGER;
	else if (strcmp("solexa", name) == 0)
		return SOLEXA;
	else if (strcmp("illumina", name) == 0)
		return ILLUMINA;
	else
		fatal_error("%s: unknown quality scale: '%s'\n", func_text, name);
	/* next line to avoid compiler warning */
	return SANGER;
}

float fastq_quality_from_letter(char letter, enum quality_scale scale) {
	float quality;
	switch (scale) {
		case SOLEXA:
		case ILLUMINA:
			quality = 10.0f * log10f(powf(10, ((float)letter - 64.0f) / 10.0) + 1.0);
			break;
		case SANGER:
		default:
			quality = (float)letter - 33.0f;
			break;
	};
	return quality;
}

void fastq_fill_qualities_from_letters(const char *letters, enum quality_scale scale, float *qualities) {
	int i;
	int len = strlen(letters);
	char letter;

	for (i = 0; i < len; ++i) {
		letter = letters[i];
		switch (scale) {
			case SOLEXA:
			case ILLUMINA:
				qualities[i] = 10.0f * log10f(powf(10, ((float)letter - 64.0f) / 10.0) + 1.0);
				break;
			case SANGER:
			default:
				qualities[i] = (float)letter - 33.0f;
			break;
		};
	}
}

double error_from_phred_quality(double Q) {
	return exp((log(10.0) * Q) / -10.0);
}
