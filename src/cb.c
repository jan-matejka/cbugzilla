#ifndef CB_CB_C
#define CB_CB_C

#include <unistd.h>

#include <libcbugzilla/cb.h>
#include <libcbugzilla/curl.h>
#include <libcbugzilla/_cb.h>
#include <libcbugzilla/bugzilla.h>


int V_MAJOR=0;
int V_MINOR=1;
int V_MICRO=0;

char *url_login = "/index.cgi";
char *url_search_list = "/";
char *url_namedcmd = "/buglist.cgi?cmdtype=runnamed&namedcmd=%s&limit=0";

int log_response(cb_t cb, char *name) {
	if(cb->http_log == NULL) {
		cb->http_log = fopen(cb->http_log_f.mem, "a");
		if(cb->http_log == NULL) {
			perror("fopen");
			return EXIT_FAILURE;
		}
	}

	fprintf(cb->http_log, "NEW %s:\n", name);
	unsigned int written = fwrite(cb->response.mem,
		sizeof(char),
		cb->response.len-1,
		cb->http_log);

	if(cb->response.len-1 != written)
		return EXIT_FAILURE;

	fprintf(cb->http_log, "\n\n");
	sync();
	return EXIT_SUCCESS;
}

/* {{{ cbi_t functions */
int cbi_free(cbi_t cbi) {
	curl_easy_cleanup(cbi->cb->curl);
	curl_global_cleanup();
	return EXIT_SUCCESS;
}

int cbi_get_recordsCount(cbi_t cbi, const char *namedcmd, int *count) {
	BO(cb_bz_login(cbi->cb))
	BO(cb_bz_RecordsCount_get(cbi->cb, namedcmd, count))

	return EXIT_SUCCESS;
}

/* {{{ setters */
int cbi_set_url(cbi_t cbi, const char *c) {
	BO(cb_string_realloc(&cbi->cb->url, strlen(c)))
	BO(cb_string_dup(&cbi->cb->url, c))
	return EXIT_SUCCESS;
}
int cbi_set_http_log_f(cbi_t cbi, const char *c) {
	BO(cb_string_realloc(&cbi->cb->http_log_f, strlen(c)))
	BO(cb_string_dup(&cbi->cb->http_log_f, c))
	return EXIT_SUCCESS;
}
int cbi_set_cookiejar_f(cbi_t cbi, const char *c) {
	BO(cb_string_realloc(&cbi->cb->cookiejar_f, strlen(c)))
	BO(cb_string_dup(&cbi->cb->cookiejar_f, c))
	return EXIT_SUCCESS;
}
int cbi_set_auth_user(cbi_t cbi, const char *c) {
	BO(cb_string_realloc(&cbi->cb->auth_user, strlen(c)))
	BO(cb_string_dup(&cbi->cb->auth_user, c))
	return EXIT_SUCCESS;
}
int cbi_set_auth_pass(cbi_t cbi, const char *c) {
	BO(cb_string_realloc(&cbi->cb->auth_pass, strlen(c)))
	BO(cb_string_dup(&cbi->cb->auth_pass, c))
	return EXIT_SUCCESS;
}
int cbi_set_verify_peer(cbi_t cbi, const int i) {
	cbi->cb->verify_peer = i;
	return EXIT_SUCCESS;
}
int cbi_set_verify_host(cbi_t cbi, const int i) {
	cbi->cb->verify_host = i;
	return EXIT_SUCCESS;
}

/* }}} setters */

cbi_t cbi_new(void) {
	cb_t cb;

	cbi_t cbi = calloc(1, sizeof(struct cbi_s));
	NULLBO(cbi);

	cbi->cb = calloc(1, sizeof(struct cb_s));
	NULLBO(cbi->cb);

	cbi->set_url          =  cbi_set_url;
	cbi->set_http_log_f   =  cbi_set_http_log_f;
	cbi->set_cookiejar_f  =  cbi_set_cookiejar_f;
	cbi->set_auth_user    =  cbi_set_auth_user;
	cbi->set_auth_pass    =  cbi_set_auth_pass;

	cbi->set_verify_peer    =  cbi_set_verify_peer;
	cbi->set_verify_host    =  cbi_set_verify_host;

	cbi->get_records_count = cbi_get_recordsCount;

	cb = cbi->cb;

	cb->verify_peer = 1;
	cb->verify_host = 1;

	cb_string_init(&cb->response);
	cb_string_init(&cb->http_log_f);
	cb_string_init(&cb->url);
	cb_string_init(&cb->auth_user);
	cb_string_init(&cb->auth_pass);

	cb_init_curl(cb);

	cbi->free = cbi_free;

	cb->log_response = log_response;
	cb->curl_perform = cb_curl_perform;

	return cbi;
}
/* }}} cbi_t functions */

#endif /* CB_CB_C */
