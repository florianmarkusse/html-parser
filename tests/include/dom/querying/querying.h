#ifndef DOM_QUERYING_QUERYING_H
#define DOM_QUERYING_QUERYING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <flo/html-parser.h>
#include <stddef.h>

typedef struct {
    const char *fileLocation;
    const flo_html_String cssQuery;
    flo_html_QueryStatus expectedStatus;
    ptrdiff_t expectedResult;
    const char *testName;
} TestFile;

unsigned char testflo_html_DomQueries(ptrdiff_t *successes, ptrdiff_t *failures,
                                      flo_html_Arena scratch);
unsigned char testQuerySelectorAll(ptrdiff_t *successes, ptrdiff_t *failures,
                                   flo_html_Arena scratch);
unsigned char testQuerySelector(ptrdiff_t *successes, ptrdiff_t *failures,
                                flo_html_Arena scratch);

#ifdef __cplusplus
}
#endif

#endif
