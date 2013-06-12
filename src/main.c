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
		{ perror("fopen: auth_file"); return EXIT_FAILURE; }

	if(0 == fread(&buf, sizeof(char), 256, fp))
		return EXIT_FAILURE;

	tok = strtok(buf, "\n");

	BO(cbi->set_auth_user(cbi, tok))

	tok = strtok(NULL, "\n");
	BO(cbi->set_auth_pass(cbi, tok))

	return EXIT_SUCCESS;
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
			return EXIT_SUCCESS;

		case 'V':
			printf("%s\n", version);
			return EXIT_SUCCESS;

		case ':':
		case '?':
			usage(stderr);
			return EXIT_FAILURE;
		}
	}

	if(optind+1 != argc)
		{ usage(stderr); return EXIT_FAILURE; }

	xdgHandle *xdg = malloc(sizeof(xdgHandle));
	xdg = xdgInitHandle(xdg);

	char *auth_file    = xdgConfigFind("cbugzilla/auth", xdg);
	char *response_log = xdgDataFind("cbugzilla/response.log", xdg);
	char *cookiejar    = xdgDataFind("cbugzilla/cookiejar", xdg);

	cbi_t cbi = NULL;
	cbi = cbi_new();

	if(strlen(auth_file) == 0) {
		fprintf(stderr, "missing auth file\n");
		return EXIT_FAILURE;
	}

	BO(authRead(cbi, auth_file))

	if(strlen(response_log) > 0)
		BO(cbi->set_http_log_f(cbi, response_log))

	BO(cbi->set_url(cbi, "https://bugs.gentoo.org"))
	BO(cbi->set_cookiejar_f(cbi, cookiejar))

	int records;
	BO(cbi->get_records_count(cbi, argv[optind], &records))

	printf("%d\n", records);

	cbi->free(cbi);
	return EXIT_SUCCESS;
}
