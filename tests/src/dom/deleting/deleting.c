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
    char *fileLocation1;
    char *fileLocation2;
    flo_String cssQuery;
    DeletionType deletionType;
    char *testName;
} TestFile;

static TestFile testFiles[] = {
    {TEST_FILE_1_BEFORE, TEST_FILE_1_AFTER, FLO_STRING("span[required] > p"),
     DELETE_NODE, "node without children and only child removal"},
    {TEST_FILE_2_BEFORE, TEST_FILE_2_AFTER, FLO_STRING("!DOCTYPE"), DELETE_NODE,
     "only document node removal"},
    {TEST_FILE_3_BEFORE, TEST_FILE_3_AFTER, FLO_STRING("!DOCTYPE"), DELETE_NODE,
     "first document node removal"},
    {TEST_FILE_4_BEFORE, TEST_FILE_4_AFTER, FLO_STRING("#text-content-test"),
     DELETE_NODE, "node with children and middle child"},
    {TEST_FILE_5_BEFORE, TEST_FILE_5_AFTER, FLO_STRING("input[text=free]"),
     DELETE_NODE, "node without children and last child"},
    {TEST_FILE_6_BEFORE, TEST_FILE_6_AFTER, FLO_STRING("#my-first-div"),
     DELETE_NODE, "node with children and first child"},
    {TEST_FILE_7_BEFORE, TEST_FILE_7_AFTER, FLO_STRING("html"), DELETE_NODE,
     "last document node removal"},
    {TEST_FILE_8_BEFORE, TEST_FILE_8_AFTER, FLO_STRING("html"), DELETE_NODE,
     "middle document node removal"},
    {TEST_FILE_9_BEFORE, TEST_FILE_9_AFTER, FLO_STRING("head"), DELETE_CHILDREN,
     "remove document node with text node"},
    {TEST_FILE_10_BEFORE, TEST_FILE_10_AFTER, FLO_STRING("head > title"),
     DELETE_CHILDREN, "remove text node"},
    {TEST_FILE_11_BEFORE, TEST_FILE_11_AFTER, FLO_STRING("body"),
     DELETE_CHILDREN, "remove body's children"},
};

static ptrdiff_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus testDeletions(flo_String fileLocation1,
                                flo_String fileLocation2,
                                flo_String cssQuery,
                                DeletionType deletionType,
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

    switch (deletionType) {
    case DELETE_NODE: {
        flo_html_removeNode(foundNode, comparisonTest.actual);
        break;
    }
    case DELETE_CHILDREN: {
        flo_html_removeChildren(foundNode, comparisonTest.actual);
        break;
    }
    default: {
        return failWithMessage(
            FLO_STRING("No suitable DeletionType was supplied!\n"));
    }
    }

    return compareAndEndTest(&comparisonTest, scratch);
}

bool testflo_html_DomDeletions(ptrdiff_t *successes, ptrdiff_t *failures,
                               flo_Arena scratch) {
    printTestTopicStart("DOM deletions");

    ptrdiff_t localSuccesses = 0;
    ptrdiff_t localFailures = 0;

    for (ptrdiff_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];

        printTestStart(testFile.testName);

        if (testDeletions(FLO_STRING_LEN(testFile.fileLocation1,
                                         strlen(testFile.fileLocation1)),
                          FLO_STRING_LEN(testFile.fileLocation2,
                                         strlen(testFile.fileLocation2)),
                          testFile.cssQuery, testFile.deletionType,
                          scratch) != TEST_SUCCESS) {
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
