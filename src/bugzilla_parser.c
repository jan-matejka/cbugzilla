#include "htmltidy.c"
#include <stdio.h>

/* Traverse the document tree */
void CGB_SavedQueries_parse(TidyDoc doc, TidyNode body ) {
	TidyNode ch1, ch2;
	ctmbstr s;
	TidyAttr tattr;
	int step=0;

	ch1 = tidyGetChild(body);

	/* path where the saved query links are:
	 *	html body div#footer ul#useful-links li#links-saved ul.links
	 *
	 * in there here's a single link:
	 *	li a
	 */

	while(1) {
		switch(step) {
			case 0:
				// Find div#footer
				ch2 = tidyGetNext(ch1);
				if(TidyTag_DIV == tidyNodeGetId(ch2)) {
					tattr = tidyAttrGetById(ch2, TidyAttr_ID);
					if(0 == strcmp("footer", tidyAttrValue(tattr))) {
						ch1 = tidyGetChild(ch2);
						step++;
					}else
						ch1 = ch2;
				}else
					ch1 = ch2;
			break;
			case 1:
				// find ul#useful-links

			break;

			default:
				fprintf(stderr, "DOM traversal failed\n");
		}
	}
}

int CGB_parse_recordsCount(TidyDoc doc, TidyNode body, int *count) {
	/* TODO: the number is value of path:
	 *	body div#bugzilla-body span.bz_result_count
	 */
	TidyNode ch1, ch2;
	ctmbstr s;
	TidyAttr tattr;
	int step=0;

	ch1 = tidyGetChild(body);

	while(1) {
		switch(step) {
			case 0:
				tattr = tidyAttrGetById(ch1, TidyAttr_ID);
				if(NULL == (s = tidyAttrValue(tattr)))
					{ goto _next; }
				if(0 != strcmp("bugzilla-body", s))
					{ goto _next; }

				ch1 = tidyGetChild(ch1);
				step++;
			break;
			case 1:
				if(TidyTag_SPAN != tidyNodeGetId(ch1))
					{ goto _next; }

				tattr = tidyAttrGetById(ch1, TidyAttr_CLASS);
				if(NULL == (s = tidyAttrValue(tattr)))
					{ goto _next; }

				if(0 != strcmp("bz_result_count", s))
					{ goto _next; }

				*count = 666;
				return EXIT_SUCCESS;
				break;

			default:
				fprintf(stderr, "DOM traversal failed\n");
				return EXIT_FAILURE;
		}

		_next:
			ch2 = tidyGetNext(ch1);
			ch1 = ch2;
	}
}
