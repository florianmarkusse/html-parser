#ifndef COMPARISON_TEST_H
#define COMPARISON_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <flo/html-parser/dom/dom.h>

#include "flo/html-parser/comparison-status.h"
#include "test-status.h"

typedef struct {
    flo_html_Dom *actual;
    flo_html_Dom *expected;
} ComparisonTest;

ComparisonTest initComparisonTest(flo_html_String startFileLocation,
                                  flo_html_String expectedFileLocation,
                                  flo_html_Arena *perm);

TestStatus compareAndEndTest(ComparisonTest *comparisonTest,
                             flo_html_Arena scratch);
TestStatus compareWithCodeAndEndTest(ComparisonTest *comparisonTest,
                                     flo_html_ComparisonStatus expectedStatus,
                                     flo_html_Arena scratch);

TestStatus getNodeFromQuerySelector(flo_html_String cssQuery,
                                    ComparisonTest *comparisonTest,
                                    flo_html_node_id *foundNode,
                                    flo_html_Arena scratch);
TestStatus failWithMessageAndCode(flo_html_String failureMessage,
                                  TestStatus failureStatus);

TestStatus failWithMessage(flo_html_String failureMessage);

#ifdef __cplusplus
}
#endif

#endif
