#include <stdlib.h>
#include <string.h>
#include <basedir.h>
#include <basedir_fs.h>
#include <unistd.h>
#include <getopt.h>

#include <libcbugzilla/cb.h>

void usage(FILE *stream) {
	fprintf(stream, "Usage: cbugzilla <namedcmd>\n"
		"       cbugzilla [-h|-v]\n"
		"\n"
		"Opptions:\n"
		"  -h --help            print help\n"
		"  -V --version         print version\n"
		"\n"
		"Bugzilla Authentication:\n"
		"  $XDG_CONFIG_HOME/cbugzilla/auth\n"
		"  in format\n"
		"     <username>\\n<password>\n"
		);
}

char shortopt[] = "Vh";
struct option long_options[] = {
	{"help", 0, 0, 'h'},
	{"version", 0, 0, 'V'},
	{0, 0, 0, 0}
};

int authRead(cbi_t cbi, char *auth_file)
{
	FILE *fp;
	char buf[256];
	char *tok;

	fp = fopen(auth_file,"r");
	if(!fp)
		{ perror("fopen: auth_file"); return CB_E; }

	if(0 == fread(&buf, sizeof(char), 256, fp))
		return CB_E;

	tok = strtok(buf, "\n");

	CB_BO(cbi->set_auth_user(cbi, tok));

	tok = strtok(NULL, "\n");
	CB_BO(cbi->set_auth_pass(cbi, tok));

	return CB_SUCCESS;
}

int main(int argc, char **argv)
{
	int option_index, opt;

	while ((option_index = -1) ,
	       (opt=getopt_long(argc, argv,
				shortopt, long_options,
				&option_index)) != -1) {
		switch(opt) {
		case 'h':
			usage(stdout);
			return CB_SUCCESS;

		case 'V':
			printf("%s\n", version);
			return CB_SUCCESS;

		case ':':
		case '?':
			usage(stderr);
			return CB_E;
		}
	}

	if(optind+1 != argc)
		{ usage(stderr); return CB_E; }

	xdgHandle *xdg = malloc(sizeof(xdgHandle));
	xdg = xdgInitHandle(xdg);

	char *auth_file    = xdgConfigFind("cbugzilla/auth", xdg);
	char *response_log = xdgDataFind("cbugzilla/response.log", xdg);
	char *cookiejar    = xdgDataFind("cbugzilla/cookiejar", xdg);

	cbi_t cbi = NULL;
	cbi = cbi_new();

	if(NULL == cbi) {
		fprintf(stderr, "failure while creating cbi\n");
		return CB_E;
	}

	if(strlen(auth_file) == 0) {
		fprintf(stderr, "missing auth file\n");
		return CB_E;
	}

	CB_BO(authRead(cbi, auth_file));

	if(strlen(response_log) > 0)
		CB_BO(cbi->set_http_log_f(cbi, response_log));

	CB_BO(cbi->set_url(cbi, "https://bugs.gentoo.org"));
	CB_BO(cbi->set_cookiejar_f(cbi, cookiejar));

	int res;
	if(0 < (res = cbi->init_curl(cbi))) {
		fprintf(stderr, "cbi->init_curl failed\n");
		if(res == CB_ECURL)
			fprintf(stderr, "curl failed: %s\n", curl_easy_strerror(cbi->get_curl_code(cbi)));
		return CB_E;
	}

	int records;
	CB_BO(cbi->get_records_count(cbi, argv[optind], &records));

	printf("%d\n", records);

	cbi->free(cbi);
	return CB_SUCCESS;
}
