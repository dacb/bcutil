#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include "memory.h"
#include "error.h"
#include "util.h"
#include "gff.h"

gff_t *gffopenreadclose(char *filename) {
	FILE *fp;
	gff_t *gf;
	char line[MAX_LINE], *last, *locus, *type, *start, *end, *strand, *attrs, *attr;
	gene_t *gene = NULL;
	int lines = 0;

	if ((fp = fopen(filename, "r")) == NULL) {
		warning("unable to open GFF file '%s'\n", filename);
		return NULL;
	}

	gf = (gff_t *)new(sizeof(gff_t));
	gf->filename = strdup(filename);
	gf->genes = 0;
	gf->gene_features = NULL;

	while (fgets(line, MAX_LINE, fp) != NULL) {
		++lines;
		if (line[0] == '#') continue;

		locus = strtok_r(line, "\t", &last);
		strtok_r((char *)NULL, "\t", &last);		// ignore field
		type = strtok_r((char *)NULL, "\t", &last);
		if (strcmp(type, "gene") == 0) {
			start = strtok_r((char *)NULL, "\t", &last);
			end = strtok_r((char *)NULL, "\t", &last);
			strtok_r((char *)NULL, "\t", &last);		// ignore field
			strand = strtok_r((char *)NULL, "\t", &last);
			strtok_r((char *)NULL, "\t", &last);		// ignore field
			attrs = strtok_r((char *)NULL, "\t\n", &last);

			gf->gene_features = (gene_t *)renew(gf->gene_features, (gf->genes + 1) * sizeof(gene_t));
			gene = &gf->gene_features[gf->genes];

			gene->locus = strdup(locus);

			gene->attrs = sndup(attrs);
			
			gene->start = atoi(start);
			gene->end = atoi(end);
			gene->strand = (strand[0] == '-' ? MINUS : PLUS);
			gene->type = OTHER;

			// parse attribute line for ID
			attr = strtok_r(attrs, ";", &last);
			while(attr != NULL) {
				if (strncmp(attr, "ID=", 3) == 0) {
					gene->locus_tag = sndup("%s", &attr[3]);
					break;
				}
				attr = strtok_r((char *)NULL, ";", &last);
			}
			if (attr == NULL) {
				gene->locus_tag = sndup("%s:%d-%d", gene->locus, gene->start, gene->end);
			}

			gf->genes++;
		} else if (strcmp(type, "CDS") == 0) {
			start = strtok_r((char *)NULL, "\t", &last);
			end = strtok_r((char *)NULL, "\t", &last);

			if (gene && gene->start == atoi(start) && gene->end == atoi(end))
				gene->type = CDS;
		} else
			continue;
	}

	fclose(fp);

	return gf;
}

