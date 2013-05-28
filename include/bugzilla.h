#ifndef CGB_BUGZILLA_H
#define CGB_BUGZILLA_H

#include <stdlib.h>
#include <string.h>

#include "cgb_curl.h"
#include "cgb_htmltidy.h"
#include "bugzilla_parser.h"

int CGB_bz_login(CGB_t * cgb);
int CGB_bz_RecordsCount_get(CGB_t *cgb, const char *namedcmd, int *count);

#endif /* CGB_BUGZILLA_H */
