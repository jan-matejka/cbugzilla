#ifndef CGB_HTMLTIDY_C
#define CGB_HTMLTIDY_C

#include <tidy.h>
#include <buffio.h>

TidyDoc CGB_tdoc_init(TidyDoc d) {
	d = tidyCreate();

	tidyOptSetBool( d, TidyQuiet, yes );
	tidyOptSetBool( d, TidyShowWarnings, no );
	return d;
}

int CGB_tidy_loadBuf(TidyDoc *d, void *buf, size_t buflen) {
	int err;

	*d = CGB_tdoc_init(*d);
	TidyBuffer _buf;
	tidyBufInit(&_buf);
	tidyBufAppend(&_buf, buf, buflen);
	err = tidyParseBuffer(*d, &_buf);
	if(err < 0)
		return EXIT_FAILURE;
	return EXIT_SUCCESS;
}

#endif /* CGB_HTMLTIDY_C */
