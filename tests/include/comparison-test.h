#ifndef COMPARISON_TEST_H
#define COMPARISON_TEST_H

#include <flo/html-parser/dom/dom.h>
#include <flo/html-parser/type/element/elements.h>

#include "flo/html-parser/comparison-status.h"
#include "test-status.h"

typedef struct {
    flo_html_Dom startflo_html_Dom;
    flo_html_TextStore startTextStore;
    flo_html_Dom expectedflo_html_Dom;
    flo_html_TextStore expectedTextStore;
} ComparisonTest;

TestStatus initComparisonTest(ComparisonTest *comparisonTest,
                              const flo_html_String startFileLocation,
                              const flo_html_String expectedFileLocation);

TestStatus compareAndEndTest(ComparisonTest *comparisonTest);
TestStatus compareWithCodeAndEndTest(ComparisonTest *comparisonTest,
                                     flo_html_ComparisonStatus expectedStatus);

TestStatus getNodeFromQuerySelector(const flo_html_String cssQuery,
                                    ComparisonTest *comparisonTest,
                                    flo_html_node_id *foundNode);
TestStatus failWithMessageAndCode(const flo_html_String failureMessage,
                                  TestStatus failureStatus,
                                  ComparisonTest *comparisonTest);

TestStatus failWithMessage(const flo_html_String failureMessage,
                           ComparisonTest *comparisonTest);
void destroyComparisonTest(ComparisonTest *comparisonTest);

#endif
