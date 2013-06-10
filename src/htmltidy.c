#ifndef CB_HTMLTIDY_C
#define CB_HTMLTIDY_C

#include <libcbugzilla/htmltidy.h>

TidyDoc cb_tdoc_init(TidyDoc d) {
	d = tidyCreate();

	tidyOptSetBool( d, TidyQuiet, yes );
	tidyOptSetBool( d, TidyShowWarnings, no );
	return d;
}

int cb_tidy_loadBuf(TidyDoc *d, void *buf, size_t buflen) {
	int err;

	*d = cb_tdoc_init(*d);
	TidyBuffer _buf;
	tidyBufInit(&_buf);
	tidyBufAppend(&_buf, buf, buflen);
	err = tidyParseBuffer(*d, &_buf);
	if(err < 0)
		return EXIT_FAILURE;
	return EXIT_SUCCESS;
}

#endif /* CB_HTMLTIDY_C */
