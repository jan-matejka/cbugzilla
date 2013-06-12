#ifndef CB_CB_H
#define CB_CB_H

#include <curl/curl.h>
#include <stdio.h>
#include "config.h"

#define CB_SUCCESS 0
#define CB_E 1
#define CB_ECURL 2

#define CB_ENULL(...)  if(NULL == __VA_ARGS__) return CB_E
#define CB_BO(...) if(0 != __VA_ARGS__) return CB_E
#define CB_BO_NULL(...) if(NULL == __VA_ARGS__) return NULL

#define CB_CURLE(...) if(CURLE_OK != (cb->res = __VA_ARGS__)) return CB_ECURL

char *version;

struct cb_s;
typedef struct cb_s *cb_t;

struct cbi_s;
typedef struct cbi_s *cbi_t;
struct cbi_s {
	int (*free)(cbi_t cbi);

	/* {{{ Configuration accessors */

	int (*set_url)(cbi_t cbi, const char *c);
	/*
	 * URL to bugzilla root
	 *  protocol://domain/path
	 *
	 *  specify /path only if bugzilla itself is running only at the /path.* and not before
	 */
	int (*set_http_log_f)(cbi_t cbi, const char *c);
	/* file path to log HTTP responses to */
	int (*set_cookiejar_f)(cbi_t cbi, const char *c);
	/* file path to store cookies at */
	int (*set_auth_user)(cbi_t cbi, const char *c);
	/* username to log in bugzilla with */
	int (*set_auth_pass)(cbi_t cbi, const char *c);
	/* password to log in bugzilla with */

	int (*set_verify_peer)(cbi_t cbi, const int v); /* default true */
	int (*set_verify_host)(cbi_t cbi, const int v); /* default true */

	/* }}} */

	/* {{{ action calls */

	int (*get_records_count)(cbi_t cbi, const char *namedcmd, int *count);

	/* }}} */

	/* {{{ internal stuff */
	cb_t cb;
	/* }}} */
};

cbi_t cbi_new(void);

#endif /* CB_CB_H */