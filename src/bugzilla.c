#include <libcbugzilla/bugzilla.h>
#include <libcbugzilla/_cb.h>

char *url_login = "/index.cgi";
char *url_search_list = "/";
char *url_namedcmd = "/buglist.cgi?cmdtype=runnamed&namedcmd=%s&limit=0";

int cb_bz_login(cb_t cb)
{
	struct curl_httppost *formpost=NULL;
	struct curl_httppost *lastptr=NULL;
	//struct curl_slist *headerlist=NULL;

	char *url=strdup(cb->url.mem);
	CB_ENULL(url);

	int len;
	len = (cb->url.len + strlen(url_login) + 1);
	url = realloc(url, sizeof(char) * len);
	CB_ENULL(url);

	url = strcat(url, url_login);
	CB_ENULL(url);

	CB_CURLE(curl_easy_setopt(cb->curl, CURLOPT_URL, url));

	CB_BO(curl_formadd(&formpost,
		&lastptr,
		CURLFORM_COPYNAME, "Bugzilla_login",
		CURLFORM_COPYCONTENTS, cb->auth_user.mem,
		CURLFORM_END));

	CB_BO(curl_formadd(&formpost,
		&lastptr,
		CURLFORM_COPYNAME, "Bugzilla_password",
		CURLFORM_COPYCONTENTS, cb->auth_pass.mem,
		CURLFORM_END));

	CB_CURLE(curl_easy_setopt(cb->curl, CURLOPT_HTTPPOST, formpost));
	CB_BO(cb->curl_perform(cb));

	curl_formfree(formpost);

	return CB_SUCCESS;
}

int cb_bz_RecordsCount_get(cb_t cb, const char *namedcmd, int *count) {
	/* TODO: the number of results is almost at the top of the page.
	 * So we can terminate the connection as soon as we read that number
	 * (with custom writefunction callback)
	 * and _maybe_ save some resources but probably not.
	 */
	char *url = strdup(cb->url.mem);
	CB_ENULL(url);

	char *namedcmd_e = curl_easy_escape(cb->curl, namedcmd, strlen(namedcmd));
	CB_ENULL(namedcmd_e);

	int len = strlen(url_namedcmd) -2 + strlen(namedcmd_e) +1;
	char query[len];
	if(0 > snprintf(query, len, url_namedcmd, namedcmd_e))
		return CB_E;

	url = realloc(url, sizeof(char)*(cb->url.len + strlen(query)));
	CB_ENULL(url);
	url = strcat(url, query);
	CB_ENULL(url);

	CB_CURLE(curl_easy_setopt(cb->curl, CURLOPT_URL, url));
	CB_CURLE(curl_easy_setopt(cb->curl, CURLOPT_HTTPGET, 1L));
	CB_CURLE(curl_easy_setopt(cb->curl, CURLOPT_FOLLOWLOCATION, 1));
	// BZ will respond with 302 to <oururl>&list_id=X
	// so follow it

	CB_BO(cb->curl_perform(cb));

	char *headline;
	headline = calloc(100, sizeof(char));

	if(0 > sprintf(headline, "rec: %s\n", namedcmd))
		return CB_E;

	cb->log_response(cb, headline);

	TidyDoc tdoc;
	CB_BO(cb_tidy_loadBuf(&tdoc,
		cb->response.mem,
		cb->response.size));

	CB_BO(cb_parse_recordsCount(tdoc, tidyGetBody(tdoc), count));

	return CB_SUCCESS;
}
