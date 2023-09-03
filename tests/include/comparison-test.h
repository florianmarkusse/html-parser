#ifndef COMPARISON_TEST_H
#define COMPARISON_TEST_H

#include <flo/html-parser/dom/dom.h>
#include <flo/html-parser/type/element/elements.h>

#include "flo/html-parser/comparison-status.h"
#include "test-status.h"

typedef struct {
    Dom startDom;
    DataContainer startDataContainer;
    Dom expectedDom;
    DataContainer expectedDataContainer;
} ComparisonTest;

TestStatus initComparisonTest(ComparisonTest *comparisonTest,
                              const char *startFileLocation,
                              const char *expectedFileLocation);

TestStatus compareAndEndTest(ComparisonTest *comparisonTest);
TestStatus compareWithCodeAndEndTest(ComparisonTest *comparisonTest,
                                     ComparisonStatus expectedStatus);

TestStatus getNodeFromQuerySelector(const char *cssQuery,
                                    ComparisonTest *comparisonTest,
                                    node_id *foundNode);
TestStatus failWithMessageAndCode(const char *failureMessage,
                                  TestStatus failureStatus,
                                  ComparisonTest *comparisonTest);

TestStatus failWithMessage(const char *failureMessage,
                           ComparisonTest *comparisonTest);
void destroyComparisonTest(ComparisonTest *comparisonTest);

#endif
