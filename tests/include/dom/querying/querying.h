#ifndef DOM_QUERYING_QUERYING_H
#define DOM_QUERYING_QUERYING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <flo/html-parser.h>
#include <stddef.h>

typedef struct {
    char *fileLocation;
    flo_String cssQuery;
    flo_html_QueryStatus expectedStatus;
    ptrdiff_t expectedResult;
    flo_String testName;
} TestFile;

void testflo_html_DomQueries(flo_Arena scratch);
void testQuerySelectorAll(flo_Arena scratch);
void testQuerySelector(flo_Arena scratch);

#ifdef __cplusplus
}
#endif

#endif
