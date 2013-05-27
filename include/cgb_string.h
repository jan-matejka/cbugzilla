#ifndef CGB_STRING_H
#define CGB_STRING_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct CBGString_s;
typedef struct CGBString_s CGBString_t;
struct CGBString_s {
	char *mem;
	size_t size;
	unsigned int len;
};

void CGBString_init(CGBString_t *cgbs);
int CGBString_realloc(CGBString_t *s, int len);
void CGBString_free(CGBString_t *s);
int CGBString_dup(CGBString_t *s, char *cs);

#endif /* CGB_STRING_H */
