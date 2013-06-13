#ifndef CB_CB_C
#define CB_CB_C

#include <unistd.h>

#include <libcbugzilla/cb.h>
#include <libcbugzilla/curl.h>
#include <libcbugzilla/_cb.h>
#include <libcbugzilla/bugzilla.h>

char *version = VERSION;

int log_response(cb_t cb, char *name) {
	if(cb->http_log == NULL) {
		if(cb->http_log_f.size == 0)
			return CB_SUCCESS;

		cb->http_log = fopen(cb->http_log_f.mem, "a");

		if(cb->http_log == NULL)
			return CB_E;
	}

	fprintf(cb->http_log, "NEW %s:\n", name);
	unsigned int written = 0;

	written = fwrite(cb->response.mem,
		sizeof(char),
		cb->response.len-1,
		cb->http_log);

	if(cb->response.len-1 != written)
		return CB_E;

	fprintf(cb->http_log, "\n\n");
	sync();
	return CB_SUCCESS;
}

/* {{{ cbi_t functions */
int cbi_free(cbi_t cbi) {
	curl_easy_cleanup(cbi->cb->curl);
	curl_global_cleanup();
	return CB_SUCCESS;
}

int cbi_get_recordsCount(cbi_t cbi, const char *namedcmd, unsigned long int *count) {
	if(count == NULL)
		return -EINVAL;

	CB_BO(cb_bz_login(cbi->cb));
	CB_BO(cb_bz_RecordsCount_get(cbi->cb, namedcmd, count));

	return CB_SUCCESS;
}

/* {{{ setters */
int cbi_set_url(cbi_t cbi, const char *c) {
	CB_BO(cb_string_realloc(&cbi->cb->url, strlen(c)));
	CB_BO(cb_string_dup(&cbi->cb->url, c));
	return CB_SUCCESS;
}
int cbi_set_http_log_f(cbi_t cbi, const char *c) {
	CB_BO(cb_string_realloc(&cbi->cb->http_log_f, strlen(c)));
	CB_BO(cb_string_dup(&cbi->cb->http_log_f, c));
	return CB_SUCCESS;
}
int cbi_set_cookiejar_f(cbi_t cbi, const char *c) {
	CB_BO(cb_string_realloc(&cbi->cb->cookiejar_f, strlen(c)));
	CB_BO(cb_string_dup(&cbi->cb->cookiejar_f, c));
	return CB_SUCCESS;
}
int cbi_set_auth_user(cbi_t cbi, const char *c) {
	CB_BO(cb_string_realloc(&cbi->cb->auth_user, strlen(c)));
	CB_BO(cb_string_dup(&cbi->cb->auth_user, c));
	return CB_SUCCESS;
}
int cbi_set_auth_pass(cbi_t cbi, const char *c) {
	CB_BO(cb_string_realloc(&cbi->cb->auth_pass, strlen(c)));
	CB_BO(cb_string_dup(&cbi->cb->auth_pass, c));
	return CB_SUCCESS;
}
int cbi_set_verify_peer(cbi_t cbi, const int i) {
	cbi->cb->verify_peer = i;
	return CB_SUCCESS;
}
int cbi_set_verify_host(cbi_t cbi, const int i) {
	cbi->cb->verify_host = i;
	return CB_SUCCESS;
}

/* }}} setters */

CURLcode cbi_get_curl_code(cbi_t cbi) {
	return cbi->cb->res;
}

cbi_t cbi_new(void) {
	cb_t cb;

	cbi_t cbi = calloc(1, sizeof(struct cbi_s));
	CB_BO_NULL(cbi);

	cbi->cb = calloc(1, sizeof(struct cb_s));
	CB_BO_NULL(cbi->cb);

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
	cb->verify_host = 2;

	cb->curl_verbose = 0;

	cb_string_init(&cb->response);
	cb_string_init(&cb->http_log_f);
	cb_string_init(&cb->url);
	cb_string_init(&cb->auth_user);
	cb_string_init(&cb->auth_pass);

	cbi->free = cbi_free;
	cbi->init_curl = cbi_init_curl;
	cbi->get_curl_code = cbi_get_curl_code;

	cb->http_log = NULL;
	cb->log_response = log_response;
	cb->curl_perform = cb_curl_perform;

	return cbi;
}

/* }}} cbi_t functions */

#endif /* CB_CB_C */
