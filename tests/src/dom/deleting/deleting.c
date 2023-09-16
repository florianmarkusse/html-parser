#include <flo/html-parser.h>
#include <stdbool.h>
#include <stdio.h>

#include "comparison-test.h"
#include "dom/deleting/deleting.h"
#include "test-status.h"
#include "test.h"

#define CURRENT_DIR "tests/src/dom/deleting/inputs/"
#define TEST_FILE_1_BEFORE CURRENT_DIR "test-1-before.html"
#define TEST_FILE_1_AFTER CURRENT_DIR "test-1-after.html"
#define TEST_FILE_2_BEFORE CURRENT_DIR "test-2-before.html"
#define TEST_FILE_2_AFTER CURRENT_DIR "test-2-after.html"
#define TEST_FILE_3_BEFORE CURRENT_DIR "test-3-before.html"
#define TEST_FILE_3_AFTER CURRENT_DIR "test-3-after.html"
#define TEST_FILE_4_BEFORE CURRENT_DIR "test-4-before.html"
#define TEST_FILE_4_AFTER CURRENT_DIR "test-4-after.html"
#define TEST_FILE_5_BEFORE CURRENT_DIR "test-5-before.html"
#define TEST_FILE_5_AFTER CURRENT_DIR "test-5-after.html"
#define TEST_FILE_6_BEFORE CURRENT_DIR "test-6-before.html"
#define TEST_FILE_6_AFTER CURRENT_DIR "test-6-after.html"
#define TEST_FILE_7_BEFORE CURRENT_DIR "test-7-before.html"
#define TEST_FILE_7_AFTER CURRENT_DIR "test-7-after.html"
#define TEST_FILE_8_BEFORE CURRENT_DIR "test-8-before.html"
#define TEST_FILE_8_AFTER CURRENT_DIR "test-8-after.html"
#define TEST_FILE_9_BEFORE CURRENT_DIR "test-9-before.html"
#define TEST_FILE_9_AFTER CURRENT_DIR "test-9-after.html"
#define TEST_FILE_10_BEFORE CURRENT_DIR "test-10-before.html"
#define TEST_FILE_10_AFTER CURRENT_DIR "test-10-after.html"
#define TEST_FILE_11_BEFORE CURRENT_DIR "test-11-before.html"
#define TEST_FILE_11_AFTER CURRENT_DIR "test-11-after.html"

typedef enum {
    DELETE_NODE,
    DELETE_CHILDREN,
    NUM_DELETION_TYPES,
} DeletionType;

typedef struct {
    const char *fileLocation1;
    const char *fileLocation2;
    const char *cssQuery;
    const DeletionType deletionType;
    const char *testName;
} __attribute__((aligned(64))) TestFile;

// TODO(florian): also try deleting the only node that exists
static const TestFile testFiles[] = {
    {TEST_FILE_1_BEFORE, TEST_FILE_1_AFTER, "span[required] > p", DELETE_NODE,
     "node without children and only child removal"},
    {TEST_FILE_2_BEFORE, TEST_FILE_2_AFTER, "!DOCTYPE", DELETE_NODE,
     "only document node removal"},
    {TEST_FILE_3_BEFORE, TEST_FILE_3_AFTER, "!DOCTYPE", DELETE_NODE,
     "first document node removal"},
    {TEST_FILE_4_BEFORE, TEST_FILE_4_AFTER, "#text-content-test", DELETE_NODE,
     "node with children and middle child"},
    {TEST_FILE_5_BEFORE, TEST_FILE_5_AFTER, "input[text=free]", DELETE_NODE,
     "node without children and last child"},
    {TEST_FILE_6_BEFORE, TEST_FILE_6_AFTER, "#my-first-div", DELETE_NODE,
     "node with children and first child"},
    {TEST_FILE_7_BEFORE, TEST_FILE_7_AFTER, "html", DELETE_NODE,
     "last document node removal"},
    {TEST_FILE_8_BEFORE, TEST_FILE_8_AFTER, "html", DELETE_NODE,
     "middle document node removal"},
    {TEST_FILE_9_BEFORE, TEST_FILE_9_AFTER, "head", DELETE_CHILDREN,
     "remove document node with text node"},
    {TEST_FILE_10_BEFORE, TEST_FILE_10_AFTER, "head > title", DELETE_CHILDREN,
     "remove text node"},
    {TEST_FILE_11_BEFORE, TEST_FILE_11_AFTER, "body", DELETE_CHILDREN,
     "remove body's children"},
};

static const size_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus testDeletions(const char *fileLocation1,
                                const char *fileLocation2, const char *cssQuery,
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
    case DELETE_NODE: {
        flo_html_removeNode(foundNode, &comparisonTest.startflo_html_Dom);
        break;
    }
    case DELETE_CHILDREN: {
        flo_html_removeChildren(foundNode, &comparisonTest.startflo_html_Dom);
        break;
    }
    default: {
        return failWithMessage("No suitable DeletionType was supplied!\n",
                               &comparisonTest);
    }
    }

    return compareAndEndTest(&comparisonTest);
}

bool testflo_html_DomDeletions(size_t *successes, size_t *failures) {
    printTestTopicStart("DOM deletions");

    size_t localSuccesses = 0;
    size_t localFailures = 0;

    for (size_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];

        printTestStart(testFile.testName);

        if (testDeletions(testFile.fileLocation1, testFile.fileLocation2,
                          testFile.cssQuery,
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
