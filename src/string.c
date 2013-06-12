#include <libcbugzilla/string.h>
#include <libcbugzilla/cb.h>

void cb_string_init(cb_string_t *cgbs) {
	cgbs->mem = NULL;
	cgbs->size = 0;
	cgbs->len  = 0;
}

int cb_string_realloc(cb_string_t *s, const unsigned int len) {
	unsigned int oldlen = s->len;
	s->len = (oldlen == 0) ? len+1 : len;
	// count the NULL byte only firt time

	s->size = sizeof(char) * (s->len);

	if(oldlen == 0)
		s->mem = calloc(s->len, sizeof(char));
	else
		s->mem = realloc(s->mem, s->size);

	if(s->mem == NULL)
		return CB_E;

	return CB_SUCCESS;
}

void cb_string_free(cb_string_t *s) {
	s->len = 0;
	s->size = 0;
	if(s->mem != NULL)
		free(s->mem);
	s->mem = NULL;
}

int cb_string_dup(cb_string_t *s, const char *cs) {
	cb_string_free(s);
	s->mem = strdup(cs);
	if(s->mem == NULL)
		return CB_E;

	s->len = strlen(cs);
	s->size = s->len * sizeof(char);

	return CB_SUCCESS;
}
