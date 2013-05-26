#include "curl.c"
#include "htmltidy.c"
#include "bugzilla_parser.c"
#include <stdlib.h>

int CGB_bz_login(CGB_t * cgb)
{
	struct curl_httppost *formpost=NULL;
	struct curl_httppost *lastptr=NULL;
	struct curl_slist *headerlist=NULL;

	char *url=strdup(cgb->url.mem);
	int len;
	len = (cgb->url.len + strlen(url_login) + 1);
	url = realloc(url, sizeof(char) * len);
	strcat(url, url_login);

	curl_easy_setopt(cgb->curl, CURLOPT_URL, url);

	curl_formadd(&formpost,
		&lastptr,
		CURLFORM_COPYNAME, "Bugzilla_login",
		CURLFORM_COPYCONTENTS, cgb->auth_user.mem,
		CURLFORM_END);

	curl_formadd(&formpost,
		&lastptr,
		CURLFORM_COPYNAME, "Bugzilla_password",
		CURLFORM_COPYCONTENTS, cgb->auth_pass.mem,
		CURLFORM_END);

	curl_easy_setopt(cgb->curl, CURLOPT_HTTPPOST, formpost);
	BO(CGB_curl_perform(cgb))

	curl_formfree(formpost);

	return EXIT_SUCCESS;
}

int CGB_bz_RecordsCount_get(CGB_t *cgb, char *namedcmd, int *count) {
	/* TODO: the number of results is almost at the top of the page.
	 * So we can terminate the connection as soon as we read that number
	 * (with custom writefunction callback)
	 * and _maybe_ save some resources but probably not.
	 */
	char *url = strdup(cgb->url.mem);
	int len = strlen(url_namedcmd) -2 + strlen(namedcmd) +1;
	char query[len];
	snprintf(query, len, url_namedcmd, namedcmd);
	url = realloc(url, sizeof(char)*(cgb->url.len + strlen(query)));
	strcat(url, query);

	curl_easy_setopt(cgb->curl, CURLOPT_URL, url);
	curl_easy_setopt(cgb->curl, CURLOPT_HTTPGET, 1L);
	curl_easy_setopt(cgb->curl, CURLOPT_FOLLOWLOCATION, 1);
	// BZ will respond with 302 to <oururl>&list_id=X
	// so follow it

	if(EXIT_FAILURE == CGB_curl_perform(cgb))
		return EXIT_FAILURE;

	TidyDoc tdoc;
	BO(CGB_tidy_loadBuf(&tdoc, cgb->response.mem, cgb->response.size))

	BO(CGB_parse_recordsCount(tdoc, tidyGetBody(tdoc), count))

	return EXIT_SUCCESS;
}

int CGB_SavedQueries_get(CGB_t *cgb) {
	curl_easy_setopt(cgb->curl, CURLOPT_URL, url_search_list);

	BO(CGB_curl_perform(cgb))

	TidyDoc tdoc;
	BO(CGB_tidy_loadBuf(&tdoc, cgb->response.mem, cgb->response.size ))

	CGB_SavedQueries_parse( tdoc, tidyGetBody(tdoc) );
}
