#include <string.h>
#include <math.h>

#include "util.h"
#include "hash.h"
#include "error.h"
#include "memory.h"
#include "fasta.h"
#include "nsplit.h"

struct nsplit *nsplit_init(int max_n_before_split, struct util_ptr_list *in) {
	struct nsplit *nsp;
	int i;

	nsp = (struct nsplit *)new(sizeof(struct nsplit));
	memset(nsp, 0, sizeof(struct nsplit));

	nsp->max_n_before_split = max_n_before_split;

	nsp->in = in;
	nsp->out = util_ptr_list_new();

	for (i = 0; i < nsp->in->ps; ++i) {
		int j, slen, k, n, splits = 0, just_split = 0;
		struct fasta *oldfa, *newfa = (struct fasta *)new(sizeof(struct fasta));
		util_ptr_list_add_ptr(nsp->out, newfa);
		oldfa = (struct fasta *)nsp->in->p[i];
		slen = strlen(oldfa->seq);
		newfa->header = sndup(oldfa->header);
		newfa->uid = sndup(oldfa->uid);
		newfa->desc = sndup(oldfa->desc);
		newfa->seq = (char *)new(slen * sizeof(char));
		for (k = 0, j = 0, n = 0; j < slen; ++j) {
			if (oldfa->seq[j] == 'N' || oldfa->seq[j] == 'n') {
				if (just_split)
					continue;
				++n;
			} else {
				just_split = 0;
				n = 0;
			}
			if (n == max_n_before_split) {
				int l;
				/* append terminal character */
				for (l = k; l > 0 && newfa->seq[l] == 'N'; --l)
					newfa->seq[l] = '\0';
				/* resize buffer */
				newfa->seq = (char *)renew(newfa->seq, (strlen(newfa->seq) + 1) * sizeof(char));

				k = 0;
				n = 0;
				++splits;
				just_split = 1;

				/* patch header */
				delete(newfa->header);
				delete(newfa->uid);
				delete(newfa->desc);
				newfa->uid = sndup("%s_%d", oldfa->uid, splits);
				newfa->desc = sndup("%s (contig split %d)", oldfa->desc, splits);
				newfa->header = sndup(">%s %s\n", newfa->uid, newfa->desc);

				/* create next contig */
				newfa = (struct fasta *)new(sizeof(struct fasta));
				newfa->uid = sndup("%s_%d", oldfa->uid, splits + 1);
				newfa->desc = sndup("%s (contig split %d)", oldfa->desc, splits + 1);
				newfa->header = sndup(">%s %s\n", newfa->uid, newfa->desc);
				newfa->seq = (char *)new(slen * sizeof(char));
				util_ptr_list_add_ptr(nsp->out, newfa);
			} else
				newfa->seq[k++] = oldfa->seq[j];
		}
		newfa->seq[k] = '\0';
	}

	return nsp;
}

void nsplit_destroy(struct nsplit *nsp) {
	int i;

	for (i = 0; i < nsp->out->ps; ++i) {
		struct fasta *d = (struct fasta *)nsp->out->p[i];

		fasta_delete(d);
	}
	util_ptr_list_delete(nsp->out);
	nsp->out = NULL;
}
