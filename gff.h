#ifndef _GFF_H_
#define _GFF_H_

#define MAX_LINE 102400

typedef struct gene {
	char *locus;
	enum TYPE {
		OTHER = 0,
		CDS = 1,
		tRNA = 2,
		rRNA = 3
	} type;
	int start;
	int end;
	enum STRAND {
		PLUS = 0,
		MINUS = 1
	} strand;
	char *locus_tag;
	char *attrs;
} gene_t;

typedef struct gff {
	char *filename;
	int genes;
	gene_t *gene_features;
} gff_t;

gff_t *gffopenreadclose(char *filename);

#endif /* _GFF_H_ */
