#ifndef CGB_CGB_H
#define CGB_CGB_H

#include <curl/curl.h>
#include <stdio.h>
#include "cgb_string.h"

#define VERBOSE 0

#define BO(...) if(EXIT_FAILURE == __VA_ARGS__) return EXIT_FAILURE;

int V_MAJOR;
int V_MINOR;
int V_MICRO;

char *url_login;
char *url_search_list;
char *auth_file;
char *url_namedcmd;

struct CGB_s;
typedef struct CGB_s CGB_t;
struct CGB_s {
	CURL *curl;
	CURLcode res;
	CGBString_t url;
	CGBString_t response;
	CGBString_t response_log_f;
	FILE *log_response;
	CGBString_t auth_user;
	CGBString_t auth_pass;
	CGBString_t cookiejar;
	int verify_peer;
	int verify_host;
};

CGB_t *CGB_new(void);
int CGB_init(CGB_t *cgb);
int CGB_cleanup(CGB_t *cgb);
int CGB_authRead(CGB_t * cgb, char *auth_file);
int CGB_log_response(CGB_t *cgb, char *name);

#endif /* CGB_CGB_H */
