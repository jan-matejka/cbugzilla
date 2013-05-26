#include <stdlib.h>
#include <string.h>

struct CBGString_s;
typedef struct CGBString_s CGBString_t;
struct CGBString_s {
	char *mem;
	size_t size;
	int len;
};

int CGBString_init(CGBString_t cgbs) {
	cgbs.mem = NULL;
	cgbs.size = 0;
	cgbs.len  = 0;
}

int CGBString_realloc(CGBString_t *s, int len) {
	int oldlen = s->len;
	s->len = len;
	if(oldlen == 0)
		s->len += 1; // count the NULL byte only firt time
	s->size = sizeof(char) * (s->len);
	s->mem = realloc(s->mem, s->size);
	if(s->mem == NULL) {
		perror("realloc");
		return EXIT_FAILURE;
	}
	if(oldlen == 0)
		memset(s->mem, 0, s->size);
}

void CGBString_free(CGBString_t *s) {
	s->len = 0;
	s->size = 0;
	if(s->mem != NULL)
		free(s->mem);
	s->mem = NULL;
}
