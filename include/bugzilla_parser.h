#ifndef CGB_BUGZILLA_PARSER_H
#define CGB_BUGZILLA_PARSER_H

#include <stdio.h>
#include <string.h>
#include "cgb_htmltidy.h"

/* Traverse the document tree */
int CGB_parse_recordsCount(TidyDoc doc, TidyNode body, int *count);

#endif /* CGB_BUGZILLA_PARSER_H */
