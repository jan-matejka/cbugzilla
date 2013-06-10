#include <libcbugzilla/bugzilla.h>

int cb_parse_recordsCount(TidyDoc doc, TidyNode body, int *count) {
	/* TODO: the number is value of path:
	 *	body div#bugzilla-body span.bz_result_count
	 */
	TidyNode ch1, ch2;
	ctmbstr s;
	TidyBuffer buf;
	tidyBufInit(&buf);
	TidyAttr tattr;
	int step=0;
	char *x,*y;

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

				tidyNodeGetText(doc, ch1, &buf);
				// FIXME: get only the node content
				// I tried using tidyNodeGetValue instead but got only NULL. I don't get how to do this.
				x = strchr((char *)buf.bp, '>');
				if(NULL == x)
					{ step=-1; goto _next; }
				y = strdup(x+1);
				x = strchr(y, ' ');
				*x = 0;
				*count = atoi(y);
				free(y);

				return EXIT_SUCCESS;
				break;

			default:
				fprintf(stderr, "DOM traversal failed\n");
				return EXIT_FAILURE;
		}

		_next:
			ch2 = tidyGetNext(ch1);
			if(ch2 == NULL)
				step = -1;
			ch1 = ch2;
	}
}
