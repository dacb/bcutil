#ifndef _UTIL_H
#define _UTIL_H

#include <stdio.h>
#include <wchar.h>
#include <ctype.h>
#include <stdarg.h>

int util_qsort_int_compare(const void *a, const void *b);

struct util_ptr_list {
	int	ps;
	void	**p;
};
struct util_ptr_list *util_ptr_list_new(void);
void util_ptr_list_add_ptr(struct util_ptr_list *il, void *a);
void util_ptr_list_cat_lists(struct util_ptr_list *dest, struct util_ptr_list *source);
void util_ptr_list_delete(struct util_ptr_list *il);
void util_ptr_list_add_ptr_ec(struct util_ptr_list *il, void *a);
void util_ptr_list_cat_lists_ec(struct util_ptr_list *dest, struct util_ptr_list *source);
void util_ptr_list_remove_index(struct util_ptr_list *pl, int index);

struct util_int_list {
	int	is;
	int	*i;
};
struct util_int_list *util_int_list_new(void);
void util_int_list_add_int(struct util_int_list *il, int a);
void util_int_list_cat_lists(struct util_int_list *dest, struct util_int_list *source);
void util_int_list_delete(struct util_int_list *il);
void util_int_list_add_int_ec(struct util_int_list *il, int a);
void util_int_list_cat_lists_ec(struct util_int_list *dest, struct util_int_list *source);

struct util_flt_list {
	int     fs;
	float  *f;
};
struct util_flt_list *util_flt_list_new(void);
void util_flt_list_add_flt(struct util_flt_list *il, float a);
void util_flt_list_cat_lists(struct util_flt_list *dest, struct util_flt_list *source);
void util_flt_list_delete(struct util_flt_list *il);
void util_flt_list_add_flt_ec(struct util_flt_list *il, float a);
void util_flt_list_cat_lists_ec(struct util_flt_list *dest, struct util_flt_list *source);

struct util_dbl_list {
	int	ds;
	double	*d;
};
struct util_dbl_list *util_dbl_list_new(void);
void util_dbl_list_add_dbl(struct util_dbl_list *il, double a);
void util_dbl_list_cat_lists(struct util_dbl_list *dest, struct util_dbl_list *source);
void util_dbl_list_delete(struct util_dbl_list *il);
void util_dbl_list_add_dbl_ec(struct util_dbl_list *il, double a);
void util_dbl_list_cat_lists_ec(struct util_dbl_list *dest, struct util_dbl_list *source);

struct util_str_list {
	int	ss;
	char	**s;
};
struct util_str_list *util_str_list_new(void);
void util_str_list_add_str(struct util_str_list *il, char *);
void util_str_list_cat_lists(struct util_str_list *dest, struct util_str_list *source);
void util_str_list_delete(struct util_str_list *il);
void util_str_list_add_str_ec(struct util_str_list *il, char *);
void util_str_list_cat_lists_ec(struct util_str_list *dest, struct util_str_list *source);

struct util_wcs_list {
	int	ss;
	wchar_t	**s;
};
struct util_wcs_list *util_wcs_list_new(void);
void util_wcs_list_add_wcs(struct util_wcs_list *il, wchar_t *);
void util_wcs_list_cat_lists(struct util_wcs_list *dest, struct util_wcs_list *source);
void util_wcs_list_delete(struct util_wcs_list *il);
void util_wcs_list_add_wcs_ec(struct util_wcs_list *il, wchar_t *);
void util_wcs_list_cat_lists_ec(struct util_wcs_list *dest, struct util_wcs_list *source);

struct util_stack {
	void		**data;
	unsigned int	empty;
	unsigned int	top;
	unsigned int	size;
};

struct util_stack *util_stack_new(void);
void *util_stack_pop(struct util_stack *stack);
void *util_stack_peek(struct util_stack *stack);
unsigned int util_stack_push(struct util_stack *stack, void *data);
void util_stack_destroy(struct util_stack *stack);

char *sndup(const char *fmt, ...);
wchar_t *wsndup(const wchar_t *fmt, ...);
void strrev(char *str);

#define wcscat_resize(desc, src) wcscat(desc = (wchar_t *)renew(desc, (wcslen(desc) + wcslen(src) + 1) * sizeof(wchar_t)), src);
#ifndef wcsdup
#define wcsdup(src) wcscpy((wchar_t *)new((wcslen(src) + 1) * sizeof(wchar_t)), src);
#endif
#ifndef vswprintf
int vswprintf(wchar_t *wcs, size_t maxlen, const wchar_t *format, va_list args);
#endif

wchar_t *stream_unicode_read(FILE *input, unsigned int *len);
wchar_t *promote_char_to_wchar(char *buf);
wchar_t *promote_buf_to_wchar(char *char_buf, unsigned int *len);

struct util_qsort_blob {
	int idx;
	int idx2;
	char *name;
	void *ptr;
	double dbl;
	int integer;
	char *str;
};

int util_qsort_blob_by_idx(const void *a, const void *b);
int util_qsort_blob_by_integer(const void *a, const void *b);
int util_qsort_blob_by_dbl(const void *a, const void *b);
int util_qsort_blob_by_idx_idx2(const void *a, const void *b);
int util_qsort_blob_by_integer_idx_idx2(const void *a, const void *b);
int util_qsort_blob_by_integer_and_reverse_idx_idx2(const void *a, const void *b);

#endif /* _UTIL_H */
