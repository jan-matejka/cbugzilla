#include "bugzilla.c"
#include "bugzilla_parser.c"
#include "curl.c"
#include "htmltidy.c"
#include "cgb.c"
#include "string.c"
#include <stdlib.h>
#include <string.h>
#include <basedir.h>
#include <basedir_fs.h>
#include <unistd.h>
#include <getopt.h>

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
			printf("%d.%d.%d\n", V_MAJOR, V_MINOR, V_MICRO);
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

	CGB_t *cgb;
	cgb = CGB_new();
	BO(CGB_init(cgb))
	BO(CGBString_dup(&cgb->response_log_f, response_log))
	BO(CGB_authRead(cgb, auth_file))
	BO(CGBString_dup(&cgb->url, "https://bugs.gentoo.org"))
	BO(CGBString_dup(&cgb->cookiejar, cookiejar))
	BO(CGB_init_curl(cgb))

	BO(CGB_bz_login(cgb))
	CGB_log_response(cgb, "bz_login");

	int records;
	BO(CGB_bz_RecordsCount_get(cgb, argv[optind], &records))

	printf("%d\n", records);

	CGB_cleanup(cgb);
	return EXIT_SUCCESS;
}
