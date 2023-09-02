#ifndef DOM_QUERYING_QUERYING_H
#define DOM_QUERYING_QUERYING_H

#include <flo/html-parser/dom/query/query-status.h>
#include <stddef.h>

typedef struct {
    const char *fileLocation;
    const char *cssQuery;
    QueryStatus expectedStatus;
    size_t expectedResult;
    const char *testName;
} __attribute__((aligned(64))) TestFile;

unsigned char testDomQueries(size_t *successes, size_t *failures);
unsigned char testQuerySelectorAll(size_t *successes, size_t *failures);
unsigned char testQuerySelector(size_t *successes, size_t *failures);

#endif
