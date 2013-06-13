#ifndef CB_BUGZILLA_PARSER_H
#define CB_BUGZILLA_PARSER_H

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "libcbugzilla/htmltidy.h"
#include "libcbugzilla/cb.h"

/* Traverse the document tree */
int cb_parse_recordsCount(TidyDoc doc, TidyNode body, int *count);

#endif /* CB_BUGZILLA_PARSER_H */
