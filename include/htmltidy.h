#ifndef CGB_HTMLTIDY_H
#define CGB_HTMLTIDY_H

#include <tidy.h>
#include <buffio.h>

TidyDoc cb_tdoc_init(TidyDoc d);
int cb_tidy_loadBuf(TidyDoc *d, void *buf, size_t buflen);

#endif /* CGB_HTMLTIDY_H */
