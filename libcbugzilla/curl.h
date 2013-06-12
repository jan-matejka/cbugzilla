#ifndef CB_CURL_H
#define CB_CURL_H

#include <unistd.h>
#include <string.h>

#include "libcbugzilla/string.h"
#include "libcbugzilla/cb.h"

size_t cb_curl_WMemCallback(void *ptr, size_t size, size_t nmemb, void *data);
int cbi_init_curl(cbi_t cbi);
int cb_curl_perform(cb_t cb);

#endif /* CB_CURL_H */
