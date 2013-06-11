#ifndef CB_BUGZILLA_H
#define CB_BUGZILLA_H

#include <stdlib.h>
#include <string.h>

#include "libcbugzilla/cb.h"
#include "libcbugzilla/htmltidy.h"
#include "libcbugzilla/bugzilla_parser.h"

int cb_bz_login(cb_t cb);
int cb_bz_RecordsCount_get(cb_t cb, const char *namedcmd, int *count);

#endif /* CB_BUGZILLA_H */
