#include "../src/string.c"
#include "../src/curl.c"
#include <string.h>

#define EXIT_TESTFAIL 2

int test_callback_useCase() {
	cb_string_t *s = malloc(sizeof(cb_string_t));
	cb_string_init(s);

	char *a = "a";
	char *b = "b";
	char *c = "c";

	cb_curl_WMemCallback(a, sizeof(char),  1, s);
	cb_curl_WMemCallback(b, sizeof(char),  1, s);
	cb_curl_WMemCallback(c, sizeof(char),  1, s);

	if(s->len != 4)
		return EXIT_TESTFAIL;

	if(strcmp(s->mem, "abc") != 0)
		return EXIT_TESTFAIL;

	return EXIT_SUCCESS;
}

int test_cbi_new() {
	cbi_t cbi = NULL;
	cbi = cbi_new();
	if(cbi == NULL)
		return EXIT_FAILURE;

	char a[] = "abcde";
	cbi->set_url(cbi, a);
	cbi->set_http_log_f(cbi, a);
	cbi->set_cookiejar_f(cbi, a);
	cbi->set_auth_pass(cbi, a);
	cbi->set_auth_user(cbi, a);
	cbi->set_verify_peer(cbi, 0);
	cbi->set_verify_host(cbi, 0);

	if(0 != strcmp(cbi->cb->url.mem, a))
		return EXIT_TESTFAIL;
	if(0 != strcmp(cbi->cb->http_log_f.mem, a))
		return EXIT_TESTFAIL;
	if(0 != strcmp(cbi->cb->cookiejar_f.mem, a))
		return EXIT_TESTFAIL;
	if(0 != strcmp(cbi->cb->auth_user.mem, a))
		return EXIT_TESTFAIL;
	if(0 != strcmp(cbi->cb->auth_pass.mem, a))
		return EXIT_TESTFAIL;

	if(0 != cbi->cb->verify_host)
		return EXIT_TESTFAIL;
	if(0 != cbi->cb->verify_peer)
		return EXIT_TESTFAIL;

	return EXIT_SUCCESS;
}

int main(void) {
	int rc;
	switch(rc = test_callback_useCase()) {
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

	if(rc != EXIT_SUCCESS)
		return rc;


	switch(rc = test_cbi_new()) {
		case EXIT_FAILURE:
			printf("cbi_new_useCase INTERFAILED\n");
			break;
		case EXIT_TESTFAIL:
			printf("cbi_new_useCase FAILED\n");
			break;
		case EXIT_SUCCESS:
			printf("cbi_new_useCase OK\n");
			break;
		default:
			printf("cbi_new_useCase UNDEFINED\n");
	}

	return rc;
}
