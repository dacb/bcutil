#ifndef _FASTA_H
#define _FASTA_H

#define MAX_FASTA_LINE 1024

struct fasta {
	char *header;
	char *uid;
	char *desc;
	char *seq;
};

struct util_ptr_list *fasta_read(char *filename);
struct fasta *fasta_new(const char *header);
void fasta_delete(struct fasta *seq);

#endif /* _FASTA_H */
