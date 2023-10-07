#include <flo/html-parser.h>
#include <stdbool.h>
#include <stdio.h>

#include "comparison-test.h"
#include "node/deleting/deleting.h"
#include "test-status.h"
#include "test.h"

typedef enum {
    DELETE_BOOLEAN_PROPERTY,
    DELETE_PROPERTY,
    NUM_DELETION_TYPES,
} DeletionType;

#define CURRENT_DIR "tests/src/node/deleting/inputs/"
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
    const char *propToDelete;
    const DeletionType deletionType;
    const char *testName;
} TestFile;

static const TestFile testFiles[] = {
    {TEST_FILE_1_BEFORE, TEST_FILE_1_AFTER, "!DOCTYPE", "html",
     DELETE_BOOLEAN_PROPERTY, "'html' on !DOCTYPE"},
    {TEST_FILE_2_BEFORE, TEST_FILE_2_AFTER, "input", "required",
     DELETE_BOOLEAN_PROPERTY, "'required' on input"},
    {TEST_FILE_3_BEFORE, TEST_FILE_3_AFTER, "html", "lang", DELETE_PROPERTY,
     "'lang' on html"},
};
static const ptrdiff_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus testDeletion(const flo_html_String fileLocation1,
                               const flo_html_String fileLocation2,
                               const flo_html_String cssQuery,
                               const flo_html_String propToDelete,
                               const DeletionType deletionType) {
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

    switch (deletionType) {
    case DELETE_BOOLEAN_PROPERTY: {
        flo_html_removeBooleanProperty(foundNode, propToDelete,
                                       &comparisonTest.startflo_html_Dom,
                                       &comparisonTest.startTextStore);
        break;
    }
    case DELETE_PROPERTY: {
        flo_html_removeProperty(foundNode, propToDelete,
                                &comparisonTest.startflo_html_Dom,
                                &comparisonTest.startTextStore);
        break;
    }
    default: {
        return failWithMessage(
            FLO_HTML_S("No suitable DeletionType was supplied!\n"),
            &comparisonTest);
    }
    }

    return compareAndEndTest(&comparisonTest);
}

bool testNodeDeletions(ptrdiff_t *successes, ptrdiff_t *failures) {
    printTestTopicStart("node deletions");

    ptrdiff_t localSuccesses = 0;
    ptrdiff_t localFailures = 0;

    for (ptrdiff_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];
        printTestStart(testFile.testName);

        if (testDeletion(
                FLO_HTML_S_LEN(testFile.fileLocation1,
                               strlen(testFile.fileLocation1)),
                FLO_HTML_S_LEN(testFile.fileLocation2,
                               strlen(testFile.fileLocation2)),
                FLO_HTML_S_LEN(testFile.cssQuery, strlen(testFile.cssQuery)),
                FLO_HTML_S_LEN(testFile.propToDelete,
                               strlen(testFile.propToDelete)),
                testFile.deletionType) != TEST_SUCCESS) {
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
