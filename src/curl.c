#ifndef CB_CURL_C
#define CB_CURL_C

#include <libcbugzilla/curl.h>
#include "libcbugzilla/_cb.h"

size_t
cb_curl_WMemCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
	size_t realsize = size * nmemb;

	cb_string_t *cbs = (cb_string_t *)data;
	if(size != sizeof(char))
		return -1;

	if(CB_E == cb_string_realloc(cbs, cbs->len + nmemb))
		return -1;

	cbs->mem = strncat(cbs->mem, ptr, realsize);
	if(cbs->mem == NULL)
		return -1;

	return realsize;
}

int cb_init_curl(cb_t cb) {
	if(curl_global_init(CURL_GLOBAL_SSL) > 0)
		return EXIT_FAILURE;

	cb->curl = curl_easy_init();
	if(!cb->curl)
		return EXIT_FAILURE;

	CB_CURLE(curl_easy_setopt(cb->curl, CURLOPT_SSL_VERIFYPEER, cb->verify_peer));

	CB_CURLE(curl_easy_setopt(cb->curl, CURLOPT_SSL_VERIFYHOST, cb->verify_host));

	CB_CURLE(curl_easy_setopt(cb->curl, CURLOPT_VERBOSE, cb->curl_verbose));
	CB_CURLE(curl_easy_setopt(cb->curl, CURLOPT_COOKIEJAR, cb->cookiejar_f.mem));
	// NOTE: setting just COOKIESESSION to 1 won't send the cookies in the next request :/
	//curl_easy_setopt(cb->curl, CURLOPT_COOKIESESSION, 1);

	CB_CURLE(curl_easy_setopt(cb->curl, CURLOPT_WRITEFUNCTION, cb_curl_WMemCallback));
	CB_CURLE(curl_easy_setopt(cb->curl, CURLOPT_WRITEDATA, (void *)&cb->response));
	return EXIT_SUCCESS;
}

int cb_curl_perform(cb_t cb) {
	cb_string_free(&cb->response);
	CB_CURLE(curl_easy_perform(cb->curl));

	return EXIT_SUCCESS;
}

#endif
