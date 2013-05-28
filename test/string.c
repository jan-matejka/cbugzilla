#include "../src/string.c"
#include "../src/curl.c"
#include <string.h>

#define EXIT_TESTFAIL 2

int test_callback_useCase() {
	CGBString_t *s = malloc(sizeof(CGBString_t));
	CGBString_init(s);

	char *a = "a";
	char *b = "b";
	char *c = "c";

	CGB_curl_WMemCallback(a, sizeof(char),  1, s);
	CGB_curl_WMemCallback(b, sizeof(char),  1, s);
	CGB_curl_WMemCallback(c, sizeof(char),  1, s);

	if(s->len != 4)
		return EXIT_TESTFAIL;

	if(strcmp(s->mem, "abc") != 0)
		return EXIT_TESTFAIL;

	return EXIT_SUCCESS;
}

int main(void) {
	switch(test_callback_useCase()) {
		case EXIT_FAILURE:
			printf("callback_useCase INTERFAILED\n");
			break;
		case EXIT_TESTFAIL:
			printf("callback_useCase FAILED\n");
			break;
		case EXIT_SUCCESS:
			printf("callback_useCase OK\n");
			break;
		default:
			printf("callback_useCase UNDEFINED\n");
	}
}
