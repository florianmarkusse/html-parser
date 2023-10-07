#ifndef DOM_QUERYING_QUERYING_H
#define DOM_QUERYING_QUERYING_H

#include <flo/html-parser/dom/query/query-status.h>
#include <flo/html-parser/utils/text/string.h>
#include <stddef.h>

typedef struct {
    const char *fileLocation;
    const flo_html_String cssQuery;
    flo_html_QueryStatus expectedStatus;
    ptrdiff_t expectedResult;
    const char *testName;
} TestFile;

unsigned char testflo_html_DomQueries(ptrdiff_t *successes,
                                      ptrdiff_t *failures);
unsigned char testQuerySelectorAll(ptrdiff_t *successes, ptrdiff_t *failures);
unsigned char testQuerySelector(ptrdiff_t *successes, ptrdiff_t *failures);

#endif
