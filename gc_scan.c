#include <string.h>
#include <math.h>

#include "util.h"
#include "hash.h"
#include "error.h"
#include "memory.h"
#include "gc_scan.h"

struct gc_scan *gc_scan_init(int window_size, struct util_ptr_list *seqs) {
	struct gc_scan *gcs;
	int i;

	gcs = (struct gc_scan *)new(sizeof(struct gc_scan));
	memset(gcs, 0, sizeof(struct gc_scan));

	gcs->window_size = window_size;
	gcs->seqs = seqs;
	gcs->data = util_ptr_list_new();

	for (i = 0; i < gcs->seqs->ps; ++i) {
		int j, slen;
		struct gc_scan_data *d = (struct gc_scan_data *)new(sizeof(struct gc_scan_data));
		util_ptr_list_add_ptr(gcs->data, d);
		d->seq = (struct fasta *)seqs->p[i];
		d->windows = util_ptr_list_new();
		slen = strlen(d->seq->seq);
		for (j = 0, d->gc_count = 0, d->n_count = 0; j < slen; ++j) {
			char c = d->seq->seq[j];
			if (c == 'g' || c == 'G' || c == 'c' || c == 'C')
				d->gc_count++;
			else if (c != 'a' && c != 'A' && c != 't' && c != 'T')
				d->n_count++;
		}
		d->gc = (float)d->gc_count / (float)(slen - d->n_count);
		for (j = 0; j < slen - window_size; ++j) {
			int k;
			struct gc_scan_window *w = (struct gc_scan_window *)new(sizeof(struct gc_scan_window));
			w->start = j;
			w->end = j + window_size - 1;
			w->gc_count = 0;
			w->n_count = 0;
			for (k = 0; k < window_size; ++k) {
				char c = d->seq->seq[k + j];
				if (c == 'g' || c == 'G' || c == 'c' || c == 'C')
					w->gc_count++;
				else if (c != 'a' && c != 'A' && c != 't' && c != 'T')
					w->n_count++;
			}
			w->gc = (float)w->gc_count / (float)(window_size - w->n_count);
			util_ptr_list_add_ptr(d->windows, (void *)w);
		}
	}

	return gcs;
}

void gc_scan_report(struct gc_scan *gcs) {
	int i;

	printf("#locus\tlocus_gc%%\tstart\tend\tmid\tgc_count\tn_count\tgc%%\tgc%%diff\n");
	for (i = 0; i < gcs->seqs->ps; ++i) {
		int j;
		struct gc_scan_data *d = (struct gc_scan_data *)gcs->data->p[i];
		for (j = 0; j < d->windows->ps; ++j) {
			struct gc_scan_window *w = (struct gc_scan_window *)d->windows->p[j];
			printf("%s\t%.2f\t%d\t%d\t%d\t%d\t%d\t%.2f\t%.4f\n", d->seq->uid, d->gc, w->start, w->end, ((w->end - w->start) / 2) + w->start, w->gc_count, w->n_count, w->gc, d->gc - w->gc);
		}
	}
}

void gc_scan_destroy(struct gc_scan *gcs) {
	int i;

	for (i = 0; i < gcs->seqs->ps; ++i) {
		int j;
		struct gc_scan_data *d = (struct gc_scan_data *)gcs->data->p[i];

		d->seq = NULL;
		for (j = 0; j < d->windows->ps; ++j) {
			struct gc_scan_window *w = (struct gc_scan_window *)d->windows->p[j];
			delete(w);
		}
		util_ptr_list_delete(d->windows);

		delete(d);
	}
}
