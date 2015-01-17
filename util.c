#include <string.h>
#include "util.h"
#include "error.h"
#include "memory.h"

struct util_ptr_list *util_ptr_list_new(void) {
	struct util_ptr_list *pl = (struct util_ptr_list *)new(sizeof(struct util_ptr_list));
	pl->ps = 0;
	pl->p = NULL;
	return pl;
}

void util_ptr_list_add_ptr(struct util_ptr_list *pl, void *p) {
	int a = pl->ps++;
	pl->p = (void *)renew(pl->p, pl->ps * sizeof(void *));
	pl->p[a] = p;
}

void util_ptr_list_cat_lists(struct util_ptr_list *dest, struct util_ptr_list *source) {
	int a, b, c = dest->ps + source->ps;
	dest->p = (void **)renew(dest->p, c * sizeof(void *));
	for (a = dest->ps, b = 0; b < source->ps; ++a, ++b)
		dest->p[a] = source->p[b];
	dest->ps = c;
}

void util_ptr_list_delete(struct util_ptr_list *pl) {
	if (pl == NULL) return;
	if (pl->p != NULL) delete(pl->p);
	delete(pl);
}

void util_ptr_list_remove_index(struct util_ptr_list *pl, int index) {
	if (index < 0 || index >= pl->ps) return;
	/* if necessary move memory */
	if (index != pl->ps - 1)
		memmove(&pl->p[index], pl->p[index + 1], (pl->ps - (index + 1)) * sizeof(void *));
	/* resize */
	pl->p = (void **)renew(pl->p, (--pl->ps) * sizeof(void *));
}

struct util_int_list *util_int_list_new(void) {
	struct util_int_list *il = (struct util_int_list *)new(sizeof(struct util_int_list));
	il->is = 0;
	il->i = NULL;
	return il;
}

void util_int_list_add_int(struct util_int_list *il, int i) {
	int a = il->is++;
	il->i = (int *)renew(il->i, il->is * sizeof(int));
	il->i[a] = i;
}

void util_int_list_cat_lists(struct util_int_list *dest, struct util_int_list *source) {
	int a, b, c = dest->is + source->is;
	dest->i = (int *)renew(dest->i, c * sizeof(int));
	for (a = dest->is, b = 0; b < source->is; ++a, ++b)
		dest->i[a] = source->i[b];
	dest->is = c;
}

void util_int_list_delete(struct util_int_list *il) {
	if (il == NULL) return;
	if (il->i != NULL) delete(il->i);
	delete(il);
}

struct util_flt_list *util_flt_list_new(void) {
        struct util_flt_list *fl = (struct util_flt_list *)new(sizeof(struct util_flt_list));
        fl->fs = 0;
        fl->f = NULL;
        return fl;
}

void util_flt_list_add_flt(struct util_flt_list *fl, float f) {
        int a = fl->fs++;
        fl->f = (float *)renew(fl->f, fl->fs * sizeof(float));
        fl->f[a] = f;
}

void util_flt_list_cat_lists(struct util_flt_list *dest, struct util_flt_list *source) {
	int a, b, c = dest->fs + source->fs;
	dest->f = (float *)renew(dest->f, c * sizeof(float));
	for (a = dest->fs, b = 0; b < source->fs; ++a, ++b)
		dest->f[a] = source->f[b];
	dest->fs = c;
}

void util_flt_list_delete(struct util_flt_list *fl) {
        if (fl == NULL) return;
        if (fl->f != NULL) delete(fl->f);
        delete(fl);
}

struct util_dbl_list *util_dbl_list_new(void) {
        struct util_dbl_list *dl = (struct util_dbl_list *)new(sizeof(struct util_dbl_list));
        dl->ds = 0;
        dl->d = NULL;
        return dl;
}

void util_dbl_list_add_dbl(struct util_dbl_list *dl, double f) {
        int a = dl->ds++;
        dl->d = (double *)renew(dl->d, dl->ds * sizeof(double));
        dl->d[a] = f;
}

void util_dbl_list_cat_lists(struct util_dbl_list *dest, struct util_dbl_list *source) {
	int a, b, c = dest->ds + source->ds;
	dest->d = (void *)renew(dest->d, c * sizeof(void *));
	for (a = dest->ds, b = 0; b < source->ds; ++a, ++b)
		dest->d[a] = source->d[b];
	dest->ds = c;
}

void util_dbl_list_delete(struct util_dbl_list *dl) {
        if (dl == NULL) return;
        if (dl->d != NULL) delete(dl->d);
        delete(dl);
}

struct util_str_list *util_str_list_new(void) {
        struct util_str_list *sl = (struct util_str_list *)new(sizeof(struct util_str_list));
        sl->ss = 0;
        sl->s = NULL;
        return sl;
}

void util_str_list_add_str(struct util_str_list *sl, char *p) {
        int a = sl->ss++;
        sl->s = (char **)renew(sl->s, sl->ss * sizeof(char *));
        sl->s[a] = p;
}

void util_str_list_cat_lists(struct util_str_list *dest, struct util_str_list *source) {
	int a, b, c = dest->ss + source->ss;
	dest->s = (char **)renew(dest->s, c * sizeof(char *));
	for (a = dest->ss, b = 0; b < source->ss; ++a, ++b)
		dest->s[a] = source->s[b];
	dest->ss = c;
}

void util_str_list_delete(struct util_str_list *sl) {
        if (sl == NULL) return;
        if (sl->s != NULL) delete(sl->s);
        delete(sl);
}

struct util_wcs_list *util_wcs_list_new(void) {
        struct util_wcs_list *sl = (struct util_wcs_list *)new(sizeof(struct util_wcs_list));
        sl->ss = 0;
        sl->s = NULL;
        return sl;
}

void util_wcs_list_add_wcs(struct util_wcs_list *sl, wchar_t *p) {
        int a = sl->ss++;
        sl->s = (wchar_t **)renew(sl->s, sl->ss * sizeof(wchar_t *));
        sl->s[a] = p;
}

void util_wcs_list_cat_lists(struct util_wcs_list *dest, struct util_wcs_list *source) {
	int a, b, c = dest->ss + source->ss;
	dest->s = (wchar_t **)renew(dest->s, c * sizeof(wchar_t *));
	for (a = dest->ss, b = 0; b < source->ss; ++a, ++b)
		dest->s[a] = source->s[b];
	dest->ss = c;
}

void util_wcs_list_delete(struct util_wcs_list *sl) {
        if (sl == NULL) return;
        if (sl->s != NULL) delete(sl->s);
        delete(sl);
}

struct util_stack *util_stack_new(void) {
	struct util_stack	*tmp;
	tmp = (struct util_stack *)new(sizeof(struct util_stack));
	memset(tmp, 0, sizeof(struct util_stack));
	tmp->empty = 1;
	return tmp;
}

void *util_stack_pop(struct util_stack *stack) {
	int	pop;
	if (stack->empty)
		return NULL;
	if (stack->top > 0)
		pop = stack->top--;
	else {
		pop = 0;
		stack->empty = 1;
	}
	return stack->data[pop];
}

void *util_stack_peek(struct util_stack *stack) {
	if (stack->empty)
		return NULL;
	return stack->data[stack->top];
}

unsigned int util_stack_push(struct util_stack *stack, void *data) {
	if (stack->empty) {		/* when stack is empty */
		if (!stack->size) {	/* when empty stack with empty data */
			stack->top = 0;
			stack->size = 1;
			stack->data = (void **)new(sizeof(void *));
		}			/* nothing needs to be done when empty stack and size > 0 */
	} else if (stack->top + 1 == stack->size) {
					/* when stack top is at size limit */
			stack->top = stack->size++;
			stack->data = (void **)renew(stack->data, stack->size * sizeof(void *));
	} else {			/* just increment top when stack size is greater than top */
		stack->top++;
	}

	stack->data[stack->top] = data;	/* copy data */
	stack->empty = 0;		/* flip empty flag */
	return stack->top + 1;		/* return # of items on stack */
}

void util_stack_destroy(struct util_stack *tmp) {
	delete(tmp->data);
	delete(tmp);
}

char *sndup(const char *fmt, ...) {
	char	*buf;
	int	size = strlen(fmt) * sizeof(char);
	int	ret;
	va_list	va_l;

	buf = (char *)new(size);

	while (1) {
		va_start(va_l, fmt);
		ret = vsnprintf(buf, size, fmt, va_l);
		va_end(va_l);

		if (ret > -1 && ret < size)
			return buf;

		if (ret > -1)
			size = ret + 1;
		else
			size *= 2;

		buf = (char *)renew(buf, size);
	}
}

wchar_t *wsndup(const wchar_t *fmt, ...) {
	wchar_t	*buf = NULL;
	size_t	chars = wcslen(fmt);
	size_t	size = chars * sizeof(wchar_t);
	int	ret;
	va_list	va_l;

	buf = (wchar_t *)new(size);

	while (1) {
		va_start(va_l, fmt);
		ret = vswprintf(buf, chars, fmt, va_l);
		va_end(va_l);

		if (ret > -1 && ret < chars)
			return buf;

		if (ret > -1) {
			chars = ret + 1;
			size = chars * sizeof(wchar_t);
		} else {
			chars *= 2;
			size = chars * sizeof(wchar_t);
		}

		buf = (wchar_t *)renew(buf, size);
	}
}

int util_qsort_blob_by_idx(const void *a, const void *b) {
        struct util_qsort_blob *A = (struct util_qsort_blob *)a, *B = (struct util_qsort_blob *)b;

        if (A->idx < B->idx)
                return -1;
        else if (A->idx == B->idx)
                return 0;
        else
                return 1;
}

int util_qsort_blob_by_integer(const void *a, const void *b) {
        struct util_qsort_blob *A = (struct util_qsort_blob *)a, *B = (struct util_qsort_blob *)b;

        if (A->integer < B->integer)
                return -1;
        else if (A->integer == B->integer)
                return 0;
        else
                return 1;
}

int util_qsort_blob_by_dbl(const void *a, const void *b) {
        struct util_qsort_blob *A = (struct util_qsort_blob *)a, *B = (struct util_qsort_blob *)b;

        if (A->dbl < B->dbl)
                return -1;
        else if (A->dbl == B->dbl)
                return 0;
        else
                return 1;
}

int util_qsort_blob_by_idx_idx2(const void *a, const void *b) {
        struct util_qsort_blob *A = (struct util_qsort_blob *)a, *B = (struct util_qsort_blob *)b;

        if (A->idx < B->idx)
                return -1;
        else if (A->idx == B->idx) {
		if (A->idx2 < B->idx2)
			return -1;
		else if (A->idx2 == B->idx2)
                	return 0;
		else
			return 1;
        } else
                return 1;
}

int util_qsort_blob_by_integer_idx_idx2(const void *a, const void *b) {
        struct util_qsort_blob *A = (struct util_qsort_blob *)a, *B = (struct util_qsort_blob *)b;

        if (A->integer < B->integer)
                return -1;
        else if (A->integer == B->integer) {
		if (A->idx < B->idx)
			return -1;
		else if (A->idx == A->idx) {
			if (A->idx2 < B->idx2)
				return -1;
			else if (A->idx2 == B->idx2)
				return 0;
			else
				return 1;
		} else
			return 1;
	} else
                return 1;
}

int util_qsort_blob_by_integer_and_reverse_idx_idx2(const void *a, const void *b) {
        struct util_qsort_blob *A = (struct util_qsort_blob *)a, *B = (struct util_qsort_blob *)b;

        if (A->integer < B->integer)
                return -1;
        else if (A->integer == B->integer) {
		if (A->idx < B->idx)
			return 1;
		else if (A->idx == A->idx) {
			if (A->idx2 < B->idx2)
				return 1;
			else if (A->idx2 == B->idx2)
				return 0;
			else
				return -1;
		} else
			return -1;
	} else
                return 1;
}

