#ifndef _SPLIT_H_
#define _SPLIT_H_

struct nsplit {
	int max_n_before_split;
	struct util_ptr_list *in;
	struct util_ptr_list *out;
};

struct nsplit *nsplit_init(int max_n_before_split, struct util_ptr_list *in);
void nsplit_destroy(struct nsplit *nsp);

#endif /* _SPLIT_H_ */
