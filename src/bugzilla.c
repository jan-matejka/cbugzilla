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
	int len;
	len = (cb->url.len + strlen(url_login) + 1);
	url = realloc(url, sizeof(char) * len);
	strcat(url, url_login);

	curl_easy_setopt(cb->curl, CURLOPT_URL, url);

	curl_formadd(&formpost,
		&lastptr,
		CURLFORM_COPYNAME, "Bugzilla_login",
		CURLFORM_COPYCONTENTS, cb->auth_user.mem,
		CURLFORM_END);

	curl_formadd(&formpost,
		&lastptr,
		CURLFORM_COPYNAME, "Bugzilla_password",
		CURLFORM_COPYCONTENTS, cb->auth_pass.mem,
		CURLFORM_END);

	curl_easy_setopt(cb->curl, CURLOPT_HTTPPOST, formpost);
	BO(cb->curl_perform(cb))

	curl_formfree(formpost);

	return EXIT_SUCCESS;
}

int cb_bz_RecordsCount_get(cb_t cb, const char *namedcmd, int *count) {
	/* TODO: the number of results is almost at the top of the page.
	 * So we can terminate the connection as soon as we read that number
	 * (with custom writefunction callback)
	 * and _maybe_ save some resources but probably not.
	 */
	char *url = strdup(cb->url.mem);
	char *namedcmd_e = curl_easy_escape(cb->curl, namedcmd, strlen(namedcmd));
	int len = strlen(url_namedcmd) -2 + strlen(namedcmd_e) +1;
	char query[len];
	snprintf(query, len, url_namedcmd, namedcmd_e);
	url = realloc(url, sizeof(char)*(cb->url.len + strlen(query)));
	strcat(url, query);

	curl_easy_setopt(cb->curl, CURLOPT_URL, url);
	curl_easy_setopt(cb->curl, CURLOPT_HTTPGET, 1L);
	curl_easy_setopt(cb->curl, CURLOPT_FOLLOWLOCATION, 1);
	// BZ will respond with 302 to <oururl>&list_id=X
	// so follow it

	if(EXIT_FAILURE == cb->curl_perform(cb))
		return EXIT_FAILURE;

	char *headline;
	headline = malloc(100 *sizeof(char));
	memset(headline, 0, 100*sizeof(char));
	if(0 > sprintf(headline, "rec: %s\n", namedcmd))
		fprintf(stderr,  "failure writing response.log\n");

	cb->log_response(cb, headline);

	TidyDoc tdoc;
	BO(cb_tidy_loadBuf(&tdoc,
		cb->response.mem,
		cb->response.size))

	BO(cb_parse_recordsCount(tdoc, tidyGetBody(tdoc), count))

	return EXIT_SUCCESS;
}
