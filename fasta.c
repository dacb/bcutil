#include <string.h>
#include <zlib.h>

#include "util.h"
#include "error.h"
#include "memory.h"
#include "fasta.h"

void fasta_delete(struct fasta *f) {
	if (f->uid)
		delete(f->uid);
	if (f->desc)
		delete(f->desc);
	if (f->seq)
		delete(f->seq);
	delete(f);
}

struct fasta *fasta_new(const char *header) {
	int i, j, hlen;
	struct fasta *f = (struct fasta *)new(sizeof(struct fasta));
	f->header = strdup(header);
	hlen = strlen(f->header);
	/* find space between first word after > and second word */
	for (j = 1; j < hlen && f->header[j] != '\r' && f->header[j] != '\n' && f->header[j] != ' ' && f->header[j] != '\t'; ++j);
	/* allocate unique id and copy from > to space */
	f->uid = (char *)new(j * sizeof(char));
	for (i = 1; i < j; ++i)
		f->uid[i - 1] = f->header[i];
	f->uid[i - 1] = '\0';
	/* find end of spaces after first word */
	for (; j < hlen && (f->header[j] == ' ' || f->header[j] == '\t'); ++j);
	/* allocate description and copy */
	f->desc = (char *)new(((hlen - j) + 1) * sizeof(char));
	for (i = j; i < hlen; ++i) {
		if (f->header[i] != '\n' && f->header[i] != '\r')
			f->desc[i - j] = f->header[i];
		else
			f->desc[i - j] = '\0';
	}
	f->desc[i - j] = '\0';
	/* sequence is null for now */
	f->seq = NULL;
	return f;
}

struct util_ptr_list *fasta_read(char *filename) {
	char *func_text = "fasta_read";
	gzFile *gz;
	struct util_ptr_list *seqs;
	char line[MAX_FASTA_LINE], line_sub[MAX_FASTA_LINE];
	int in_seq = 0, line_no = 0, current_size, seqlen;
	struct fasta *seq;

	if ((gz = gzopen(filename, "rb")) == NULL) {
		fatal_error("%s: unable to open file '%s'\n", func_text, filename);
	}

	seqs = util_ptr_list_new();

	while (gzgets(gz, line, MAX_FASTA_LINE) != NULL) {
		line_no++;
		if (line[0] == '\n')
			continue;
		else if (line[0] == '>') {
			int i;
			if (in_seq)
				util_ptr_list_add_ptr(seqs, seq);
			seq = fasta_new(line);
			current_size = MAX_FASTA_LINE;
			seq->seq = (char *)new(sizeof(char) * current_size);
			seq->seq[0] = '\0';
			seqlen = 0;
			in_seq = 1;
#if 0
			/* enforce uniqueness of uid */
			for (i = 0; i < seqs->ps; ++i) {
				struct fasta *f = seqs->p[i];
				if (strcmp(f->uid, seq->uid) == 0) {
					warning("%s: ignored entry with duplicate unique gene ID (%s) on line %d\n", func_text, f->uid, line_no);
					fasta_delete(seq);
					in_seq = 0;
					break;
				}
			}
#endif
		} else {
			if (in_seq) {
				int i, j, k, line_len = strlen(line);
				size_t new_size;
				/* find first non whitespace */
				for (j = 0; j < line_len && (line[j] == ' ' || line[j] == '\t'); ++j);
				/* copy from first non whitespace to last non whitespace */
				for (i = j; i < line_len && !isspace(line[i]); ++i)
					line_sub[i - j] = line[i];
				line_sub[i - j] = '\0';
				new_size = ((i - j) + seqlen + 1) * sizeof(char);
				if (new_size > current_size) {
					while (new_size > current_size)
						current_size *= 2;
					seq->seq = (char *)renew(seq->seq, current_size);
				}
				//strcat(seq->seq, line_sub);
				for (k = 0; k < (i - j); ++k) {
					seq->seq[k + seqlen] = line_sub[k];
				}
				seqlen = k + seqlen;
			} else
				warning("%s: ignored contents of line no. %d\n", func_text, line_no);
		}
	}
	if (in_seq)
		util_ptr_list_add_ptr(seqs, seq);

	gzclose(gz);

	printf("fasta_read: found %d sequences in '%s'\n", seqs->ps, filename);

	return seqs;
}
