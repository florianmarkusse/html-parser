#ifndef QUERYING_QUERYING_H
#define QUERYING_QUERYING_H

#include <flo/html-parser/dom/query/dom-query-status.h> //`
#include <stddef.h>

typedef struct {
    const char *fileLocation;
    const char *cssQuery;
    QueryStatus expectedStatus;
    size_t expectedResult;
    const char *testName;
} __attribute__((aligned(64))) TestFile;

unsigned char testQueries(size_t *successes, size_t *failures);
unsigned char testQuerySelectorAll(size_t *successes, size_t *failures);
unsigned char testQuerySelector(size_t *successes, size_t *failures);

#endif
