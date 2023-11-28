#ifndef COMPARISON_TEST_H
#define COMPARISON_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <flo/html-parser/dom/dom.h>

#include "flo/html-parser/comparison-status.h"

typedef struct {
    flo_html_Dom *actual;
    flo_html_Dom *expected;
} ComparisonTest;

ComparisonTest initComparisonTest(char *startFileLocation,
                                  char *expectedFileLocation, flo_Arena *perm);

void compareAndEndTest(ComparisonTest *comparisonTest, flo_Arena scratch);
void compareWithCodeAndEndTest(ComparisonTest *comparisonTest,
                               flo_html_ComparisonStatus expectedStatus,
                               flo_Arena scratch);

bool getNodeFromQuerySelector(flo_String cssQuery,
                              ComparisonTest *comparisonTest,
                              flo_html_node_id *foundNode, flo_Arena scratch);

#ifdef __cplusplus
}
#endif

#endif
