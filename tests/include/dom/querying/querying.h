#ifndef DOM_QUERYING_QUERYING_H
#define DOM_QUERYING_QUERYING_H

#include <flo/html-parser/dom/query/query-status.h>
#include <stddef.h>

typedef struct {
    const char *fileLocation;
    const char *cssQuery;
    flo_html_QueryStatus expectedStatus;
    size_t expectedResult;
    const char *testName;
} TestFile;

unsigned char testflo_html_DomQueries(size_t *successes, size_t *failures);
unsigned char testQuerySelectorAll(size_t *successes, size_t *failures);
unsigned char testQuerySelector(size_t *successes, size_t *failures);

#endif
