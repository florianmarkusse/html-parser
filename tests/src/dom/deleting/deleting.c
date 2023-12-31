#include <flo/html-parser.h>
#include <stdbool.h>
#include <stdio.h>

#include "comparison-test.h"
#include "dom/deleting/deleting.h"
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
    flo_String testName;
} TestFile;

static TestFile testFiles[] = {
    {TEST_FILE_1_BEFORE, TEST_FILE_1_AFTER, FLO_STRING("span[required] > p"),
     DELETE_NODE, FLO_STRING("node without children and only child removal")},
    {TEST_FILE_2_BEFORE, TEST_FILE_2_AFTER, FLO_STRING("!DOCTYPE"), DELETE_NODE,
     FLO_STRING("only document node removal")},
    {TEST_FILE_3_BEFORE, TEST_FILE_3_AFTER, FLO_STRING("!DOCTYPE"), DELETE_NODE,
     FLO_STRING("first document node removal")},
    {TEST_FILE_4_BEFORE, TEST_FILE_4_AFTER, FLO_STRING("#text-content-test"),
     DELETE_NODE, FLO_STRING("node with children and middle child")},
    {TEST_FILE_5_BEFORE, TEST_FILE_5_AFTER, FLO_STRING("input[text=free]"),
     DELETE_NODE, FLO_STRING("node without children and last child")},
    {TEST_FILE_6_BEFORE, TEST_FILE_6_AFTER, FLO_STRING("#my-first-div"),
     DELETE_NODE, FLO_STRING("node with children and first child")},
    {TEST_FILE_7_BEFORE, TEST_FILE_7_AFTER, FLO_STRING("html"), DELETE_NODE,
     FLO_STRING("last document node removal")},
    {TEST_FILE_8_BEFORE, TEST_FILE_8_AFTER, FLO_STRING("html"), DELETE_NODE,
     FLO_STRING("middle document node removal")},
    {TEST_FILE_9_BEFORE, TEST_FILE_9_AFTER, FLO_STRING("head"), DELETE_CHILDREN,
     FLO_STRING("remove document node with text node")},
    {TEST_FILE_10_BEFORE, TEST_FILE_10_AFTER, FLO_STRING("head > title"),
     DELETE_CHILDREN, FLO_STRING("remove text node")},
    {TEST_FILE_11_BEFORE, TEST_FILE_11_AFTER, FLO_STRING("body"),
     DELETE_CHILDREN, FLO_STRING("remove body's children")},
};

static ptrdiff_t numTestFiles = FLO_COUNTOF(testFiles);

static void testDeletions(char *fileLocation1, char *fileLocation2,
                          flo_String cssQuery, DeletionType deletionType,
                          flo_Arena scratch) {
    ComparisonTest comparisonTest =
        initComparisonTest(fileLocation1, fileLocation2, &scratch);
    if (comparisonTest.actual == NULL) {
        return;
    }

    flo_html_node_id foundNode = 0;
    if (!getNodeFromQuerySelector(cssQuery, &comparisonTest, &foundNode,
                                  scratch)) {
        return;
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
        FLO_TEST_FAILURE {
            FLO_ERROR((FLO_STRING("No suitable DeletionType was supplied!\n")));
        }
        return;
    }
    }

    return compareAndEndTest(&comparisonTest, scratch);
}

void testflo_html_DomDeletions(flo_Arena scratch) {
    FLO_TEST_TOPIC(FLO_STRING("DOM deletions")) {
        for (ptrdiff_t i = 0; i < numTestFiles; i++) {
            TestFile testFile = testFiles[i];

            FLO_TEST(testFile.testName) {
                testDeletions(testFile.fileLocation1, testFile.fileLocation2,
                              testFile.cssQuery, testFile.deletionType,
                              scratch);
            }
        }
    }
}
