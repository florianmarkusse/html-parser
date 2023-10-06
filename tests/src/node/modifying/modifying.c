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
    const char *fileLocation1;
    const char *fileLocation2;
    const char *cssQuery;
    const char *propKey;
    const flo_html_String newPropValue;
    const char *testName;
} TestFile;

static const TestFile testFiles[] = {
    {TEST_FILE_1_BEFORE, TEST_FILE_1_AFTER, "body", "style",
     FLO_HTML_S("newstyle"), "change property value"},
    {TEST_FILE_2_BEFORE, TEST_FILE_2_AFTER, "#text-content-test", "id",
     FLO_HTML_S("id-changed"), "change id value"},
    {TEST_FILE_3_BEFORE, TEST_FILE_3_AFTER, "#text-content-test",
     "I am the new text content, bow for me!", FLO_HTML_EMPTY_STRING,
     "setting text content"},
};
static const size_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus testModification(const flo_html_String fileLocation1,
                                   const flo_html_String fileLocation2,
                                   const flo_html_String cssQuery,
                                   const flo_html_String propKey,
                                   const flo_html_String newPropValue) {
    TestStatus result = TEST_FAILURE;

    ComparisonTest comparisonTest;
    result = initComparisonTest(&comparisonTest, fileLocation1, fileLocation2);
    if (result != TEST_SUCCESS) {
        return result;
    }

    flo_html_node_id foundNode = 0;
    result = getNodeFromQuerySelector(cssQuery, &comparisonTest, &foundNode);
    if (result != TEST_SUCCESS) {
        return result;
    }

    if (newPropValue.len > 0) {
        flo_html_ElementStatus elementStatus = flo_html_setPropertyValue(
            foundNode, propKey, newPropValue, &comparisonTest.startflo_html_Dom,
            &comparisonTest.startTextStore);
        if (elementStatus != ELEMENT_SUCCESS) {
            return failWithMessage(
                FLO_HTML_S("Failed to set property value!\n"), &comparisonTest);
        }
    } else {
        flo_html_DomStatus domStatus = flo_html_setTextContent(
            foundNode, propKey, &comparisonTest.startflo_html_Dom,
            &comparisonTest.startTextStore);
        if (domStatus != DOM_SUCCESS) {
            return failWithMessage(FLO_HTML_S("Failed to set text content!\n"),
                                   &comparisonTest);
        }
    }

    return compareAndEndTest(&comparisonTest);
}

bool testNodeModifications(size_t *successes, size_t *failures) {
    printTestTopicStart("node modifications");

    size_t localSuccesses = 0;
    size_t localFailures = 0;

    for (size_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];

        printTestStart(testFile.testName);

        if (testModification(
                FLO_HTML_S_LEN(testFile.fileLocation1,
                               strlen(testFile.fileLocation1)),
                FLO_HTML_S_LEN(testFile.fileLocation2,
                               strlen(testFile.fileLocation2)),
                FLO_HTML_S_LEN(testFile.cssQuery, strlen(testFile.cssQuery)),
                FLO_HTML_S_LEN(testFile.propKey, strlen(testFile.propKey)),
                testFile.newPropValue) != TEST_SUCCESS) {
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
