
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
} __attribute__((aligned(64))) TestFile;

static const TestFile testFiles[] = {
    {TEST_FILE_1_BEFORE, TEST_FILE_1_AFTER, "!DOCTYPE", "html",
     DELETE_BOOLEAN_PROPERTY, "'html' on !DOCTYPE"},
    {TEST_FILE_2_BEFORE, TEST_FILE_2_AFTER, "input", "required",
     DELETE_BOOLEAN_PROPERTY, "'required' on input"},
    {TEST_FILE_3_BEFORE, TEST_FILE_3_AFTER, "html", "lang", DELETE_PROPERTY,
     "'lang' on html"},
};
static const size_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus testDeletion(const char *fileLocation1,
                               const char *fileLocation2, const char *cssQuery,
                               const char *propToDelete,
                               const DeletionType deletionType) {
    TestStatus result = TEST_FAILURE;

    ComparisonTest comparisonTest;
    result = initComparisonTest(&comparisonTest, fileLocation1, fileLocation2);
    if (result != TEST_SUCCESS) {
        return result;
    }

    node_id foundNode = 0;
    result = getNodeFromQuerySelector(cssQuery, &comparisonTest, &foundNode);
    if (result != TEST_SUCCESS) {
        return result;
    }

    switch (deletionType) {
    case DELETE_BOOLEAN_PROPERTY: {
        flo_html_removeBooleanProperty(foundNode, propToDelete, &comparisonTest.startDom,
                              &comparisonTest.startTextStore);
        break;
    }
    case DELETE_PROPERTY: {
        flo_html_removeProperty(foundNode, propToDelete, &comparisonTest.startDom,
                       &comparisonTest.startTextStore);
        break;
    }
    default: {
        return failWithMessage("No suitable DeletionType was supplied!\n",
                               &comparisonTest);
    }
    }

    return compareAndEndTest(&comparisonTest);
}

bool testNodeDeletions(size_t *successes, size_t *failures) {
    printTestTopicStart("node deletions");

    size_t localSuccesses = 0;
    size_t localFailures = 0;

    for (size_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];
        printTestStart(testFile.testName);

        if (testDeletion(testFile.fileLocation1, testFile.fileLocation2,
                         testFile.cssQuery, testFile.propToDelete,
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
