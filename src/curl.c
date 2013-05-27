#include "cgb_curl.h"

size_t
CGB_curl_WMemCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
	size_t realsize = size * nmemb;

	CGBString_t *cgbs = (CGBString_t *)data;
	CGBString_init(cgbs);
	if(size != sizeof(char)) {
		fprintf(stderr, "unexpected size");
		return EXIT_FAILURE;
		// FIXME: this probabl won't work
	}

	CGBString_realloc(cgbs, cgbs->len + nmemb);

	strncat(cgbs->mem, ptr, realsize);
	return realsize;
}

int CGB_init_curl(CGB_t *cgb) {
	curl_global_init(CURL_GLOBAL_SSL);

	cgb->curl = curl_easy_init();
	if(!cgb->curl)
		return EXIT_FAILURE;

	curl_easy_setopt(cgb->curl, CURLOPT_SSL_VERIFYPEER, cgb->verify_peer);
	curl_easy_setopt(cgb->curl, CURLOPT_SSL_VERIFYHOST, cgb->verify_host);

	curl_easy_setopt(cgb->curl, CURLOPT_VERBOSE, VERBOSE);
	curl_easy_setopt(cgb->curl, CURLOPT_COOKIEJAR, cgb->cookiejar.mem);
	// NOTE: setting just COOKIESESSION to 1 won't send the cookies in the next request :/
	//curl_easy_setopt(cgb->curl, CURLOPT_COOKIESESSION, 1);

	curl_easy_setopt(cgb->curl, CURLOPT_WRITEFUNCTION, CGB_curl_WMemCallback);
	curl_easy_setopt(cgb->curl, CURLOPT_WRITEDATA, (void *)&cgb->response);
	return EXIT_SUCCESS;
}

int CGB_curl_perform(CGB_t *cgb) {
	CGBString_free(&cgb->response);
	cgb->res = curl_easy_perform(cgb->curl);
	if(cgb->res != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
			curl_easy_strerror(cgb->res));
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
