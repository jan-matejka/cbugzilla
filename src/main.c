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

int main(int argc, char **argv)
{
	if(argc < 2)
		{ fprintf(stderr, "fu"); return EXIT_FAILURE; }

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
	BO(CGB_bz_RecordsCount_get(cgb, argv[1], &records))

	printf("Records for python-herd: %d\n", records);

	CGB_cleanup(cgb);
	return EXIT_SUCCESS;
}
