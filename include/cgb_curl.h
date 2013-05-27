#ifndef CGB_CURL_H
#define CGB_CURL_H

#include <curl/curl.h>
#include "cgb_string.h"
#include "cgb.h"

size_t
CGB_curl_WMemCallback(void *ptr, size_t size, size_t nmemb, void *data);
int CGB_init_curl(CGB_t *cgb);
int CGB_curl_perform(CGB_t *cgb);

#endif /* CGB_CURL_H */
