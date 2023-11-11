#include <flo/html-parser.h>
#include <stdbool.h>
#include <stdio.h>

#include "comparison-test.h"
#include "node/modifying/modifying.h"
#include "test-status.h"
#include "test.h"

#define CURRENT_DIR "tests/src/node/modifying/inputs/"
#define TEST_FILE_1_BEFORE CURRENT_DIR "test-1-before.html"
#define TEST_FILE_1_AFTER CURRENT_DIR "test-1-after.html"
#define TEST_FILE_2_BEFORE CURRENT_DIR "test-2-before.html"
#define TEST_FILE_2_AFTER CURRENT_DIR "test-2-after.html"
#define TEST_FILE_3_BEFORE CURRENT_DIR "test-3-before.html"
#define TEST_FILE_3_AFTER CURRENT_DIR "test-3-after.html"

typedef struct {
    char *fileLocation1;
    char *fileLocation2;
    char *cssQuery;
    char *propKey;
    flo_String newPropValue;
    char *testName;
} TestFile;

static TestFile testFiles[] = {
    {TEST_FILE_1_BEFORE, TEST_FILE_1_AFTER, "body", "style",
     FLO_STRING("newstyle"), "change property value"},
    {TEST_FILE_2_BEFORE, TEST_FILE_2_AFTER, "#text-content-test", "id",
     FLO_STRING("id-changed"), "change id value"},
    {TEST_FILE_3_BEFORE, TEST_FILE_3_AFTER, "#text-content-test",
     "I am the new text content, bow for me!", FLO_EMPTY_STRING,
     "setting text content"},
};
static ptrdiff_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus testModification(flo_String fileLocation1,
                                   flo_String fileLocation2,
                                   flo_String cssQuery,
                                   flo_String propKey,
                                   flo_String newPropValue,
                                   flo_Arena scratch) {
    ComparisonTest comparisonTest =
        initComparisonTest(fileLocation1, fileLocation2, &scratch);

    TestStatus result = TEST_FAILURE;
    flo_html_node_id foundNode = 0;
    result = getNodeFromQuerySelector(cssQuery, &comparisonTest, &foundNode,
                                      scratch);
    if (result != TEST_SUCCESS) {
        return result;
    }

    if (newPropValue.len > 0) {
        if (!flo_html_setPropertyValue(foundNode, propKey, newPropValue,
                                       comparisonTest.actual, &scratch)) {
            return failWithMessage(
                FLO_STRING("Failed to set property value!\n"));
        }
    } else {
        flo_html_setTextContent(foundNode, propKey, comparisonTest.actual,
                                &scratch);
    }

    return compareAndEndTest(&comparisonTest, scratch);
}

bool testNodeModifications(ptrdiff_t *successes, ptrdiff_t *failures,
                           flo_Arena scratch) {
    printTestTopicStart("node modifications");

    ptrdiff_t localSuccesses = 0;
    ptrdiff_t localFailures = 0;

    for (ptrdiff_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];

        printTestStart(testFile.testName);

        if (testModification(
                FLO_STRING_LEN(testFile.fileLocation1,
                               strlen(testFile.fileLocation1)),
                FLO_STRING_LEN(testFile.fileLocation2,
                               strlen(testFile.fileLocation2)),
                FLO_STRING_LEN(testFile.cssQuery, strlen(testFile.cssQuery)),
                FLO_STRING_LEN(testFile.propKey, strlen(testFile.propKey)),
                testFile.newPropValue, scratch) != TEST_SUCCESS) {
            localFailures++;
        } else {
            localSuccesses++;
        }
    }

    printTestScore(localSuccesses, localFailures);

    *successes += localSuccesses;
    *failures += localFailures;

    return localFailures > 0;
}
