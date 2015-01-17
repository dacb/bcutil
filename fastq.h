#ifndef _FASTQ_H
#define _FASTQ_H

#define MAX_FASTQ_LINE 1024

struct fastq {
	char *uid;
	char *seq;
	char *qual;
};

struct util_ptr_list *fastq_read(const char *filename, int warnings_are_fatal);

enum quality_scale {
	SANGER = 0,
	SOLEXA = 1,
	ILLUMINA = 2 };

enum quality_scale fastq_text_to_quality_scale(const char *name);
float fastq_quality_from_letter(char letter, enum quality_scale scale);
void fastq_fill_qualities_from_letters(const char *letters, enum quality_scale scale, float *qualities);
double error_from_phred_quality(double Q);
void fastq_delete(struct fastq *seq);

#endif /* _FASTQ_H */
