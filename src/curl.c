#ifndef CB_CURL_C
#define CB_CURL_C

#include <libcbugzilla/curl.h>
#include "libcbugzilla/_cb.h"

size_t
cb_curl_WMemCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
	size_t realsize = size * nmemb;

	cb_string_t *cbs = (cb_string_t *)data;
	if(size != sizeof(char)) {
		fprintf(stderr, "unexpected size\n");
		return EXIT_FAILURE;
		// FIXME: this probabl won't work
	}

	cb_string_realloc(cbs, cbs->len + nmemb);

	strncat(cbs->mem, ptr, realsize);
	return realsize;
}

int cb_init_curl(cb_t cb) {
	if(curl_global_init(CURL_GLOBAL_SSL) > 0)
		return EXIT_FAILURE;

	cb->curl = curl_easy_init();
	if(!cb->curl)
		return EXIT_FAILURE;

	curl_easy_setopt(cb->curl, CURLOPT_SSL_VERIFYPEER, cb->verify_peer);
	curl_easy_setopt(cb->curl, CURLOPT_SSL_VERIFYHOST, cb->verify_host);

	curl_easy_setopt(cb->curl, CURLOPT_VERBOSE, VERBOSE);
	curl_easy_setopt(cb->curl, CURLOPT_COOKIEJAR, cb->cookiejar_f.mem);
	// NOTE: setting just COOKIESESSION to 1 won't send the cookies in the next request :/
	//curl_easy_setopt(cb->curl, CURLOPT_COOKIESESSION, 1);

	curl_easy_setopt(cb->curl, CURLOPT_WRITEFUNCTION, cb_curl_WMemCallback);
	curl_easy_setopt(cb->curl, CURLOPT_WRITEDATA, (void *)&cb->response);
	return EXIT_SUCCESS;
}

int cb_curl_perform(cb_t cb) {
	cb_string_free(&cb->response);
	cb->res = curl_easy_perform(cb->curl);
	if(cb->res != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
			curl_easy_strerror(cb->res));
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

#endif
