#ifndef _GC_SCAN_H

#define _GC_SCAN_H

#include <fasta.h>

struct gc_scan {
	int window_size;
	struct util_ptr_list *seqs;
	struct util_ptr_list *data;
};

struct gc_scan_data {
	int gc_count;
	int n_count;
	float gc;
	struct fasta *seq;
	struct util_ptr_list *windows;
};

struct gc_scan_window {
	int start, end;
	int gc_count;
	int n_count;
	float gc;
};

struct gc_scan *gc_scan_init(int window_size, struct util_ptr_list *seqs);
void gc_scan_report(struct gc_scan *gcs);
void gc_scan_destroy(struct gc_scan *gcs);

#endif /* _GC_SCAN_H */
