#ifndef CB_STRING_H
#define CB_STRING_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct cb_string_s;
typedef struct cb_string_s cb_string_t;
struct cb_string_s {
	char *mem;
	size_t size;
	unsigned int len;
};

void cb_string_init(cb_string_t *cgbs);
int cb_string_realloc(cb_string_t *s, int len);
void cb_string_free(cb_string_t *s);
int cb_string_dup(cb_string_t *s, const char *cs);

#endif /* CB_STRING_H */
