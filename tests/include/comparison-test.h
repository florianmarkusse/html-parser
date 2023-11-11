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

ComparisonTest initComparisonTest(flo_String startFileLocation,
                                  flo_String expectedFileLocation,
                                  flo_Arena *perm);

TestStatus compareAndEndTest(ComparisonTest *comparisonTest,
                             flo_Arena scratch);
TestStatus compareWithCodeAndEndTest(ComparisonTest *comparisonTest,
                                     flo_html_ComparisonStatus expectedStatus,
                                     flo_Arena scratch);

TestStatus getNodeFromQuerySelector(flo_String cssQuery,
                                    ComparisonTest *comparisonTest,
                                    flo_html_node_id *foundNode,
                                    flo_Arena scratch);
TestStatus failWithMessageAndCode(flo_String failureMessage,
                                  TestStatus failureStatus);

TestStatus failWithMessage(flo_String failureMessage);

#ifdef __cplusplus
}
#endif

#endif
