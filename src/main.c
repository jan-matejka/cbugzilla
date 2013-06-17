#include <stdlib.h>
#include <string.h>
#include <basedir.h>
#include <basedir_fs.h>
#include <unistd.h>
#include <getopt.h>

#include <libcbugzilla/cb.h>
#include "config.h"

void usage(FILE *stream) {
	fprintf(stream, "Usage: cbugzilla <namedcmd>\n"
		"       cbugzilla [-h|-v]\n"
		"\n"
		"Opptions:\n"
		"  -h --help            print help\n"
		"  -V --version         print version\n"
		"  -t --times           print timing information\n"
		"\n"
		"Bugzilla Authentication:\n"
		"  $XDG_CONFIG_HOME/cbugzilla/auth\n"
		"  in format\n"
		"     <username>\\n<password>\n"
		"  note: if you need more than 255 characters you're screwed\n"
		);
}

char shortopt[] = "Vht";
struct option long_options[] = {
	{"help", 0, 0, 'h'},
	{"times", 0, 0, 't'},
	{"version", 0, 0, 'V'},
	{0, 0, 0, 0}
};

int authRead(cbi_t cbi, char *auth_file)
{
	FILE *fp;
	char buf[256];
	char *tok;

	memset(&buf, 0, 256);

	fp = fopen(auth_file,"r");
	if(!fp)
		{ perror("fopen: auth_file"); return CB_E; }

	if(0 == fread(&buf, sizeof(char), 256, fp))
		return CB_E;

	buf[255] = '\0';
	tok = strtok(buf, "\n");

	CB_BO(cbi->set_auth_user(cbi, tok));

	tok = strtok(NULL, "\n");
	CB_BO(cbi->set_auth_pass(cbi, tok));

	return CB_SUCCESS;
}

static void print_time(char *name, double delta) {
	if(delta < 0)
		printf("\t%s: error getting value\n", name);
	else
		printf("\t%s: %f\n", name, delta);
}

static void print_times(cbi_t cbi) {
	printf("timing info:\n");
	double delta;
	if(CB_SUCCESS != cbi->get_total_response_time(cbi, &delta))
		delta = -1;
	print_time("total response", delta);

	if(CB_SUCCESS != cbi->get_namelookup_time(cbi, &delta))
		delta = -1;
	print_time("namelookup", delta);

	if(CB_SUCCESS != cbi->get_pretransfer_time(cbi, &delta))
		delta = -1;
	print_time("pretransfer", delta);

	if(CB_SUCCESS != cbi->get_starttransfer_time(cbi, &delta))
		delta = -1;
	print_time("starttransfer", delta);

	if(CB_SUCCESS != cbi->get_connect_time(cbi, &delta))
		delta = -1;
	print_time("connect", delta);

	if(CB_SUCCESS != cbi->get_total_time(cbi, &delta))
		delta = -1;
	print_time("total", delta);
}

int main(int argc, char **argv)
{
	int option_index, opt;
	int times = 0;

	while ((option_index = -1) ,
	       (opt=getopt_long(argc, argv,
				shortopt, long_options,
				&option_index)) != -1) {
		switch(opt) {
		case 'h':
			usage(stdout);
			return CB_SUCCESS;

		case 'V':
			printf("%s\n", PACKAGE_VERSION);
			return CB_SUCCESS;

		case 't':
			times = 1;
			break;

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

	unsigned long int records;
	CB_BO(cbi->get_records_count(cbi, argv[optind], &records));

	printf("%ld\n", records);

	if(times)
		print_times(cbi);

	cbi->free(cbi);
	return CB_SUCCESS;
}
