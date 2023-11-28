#include <flo/html-parser.h>
#include <stdbool.h>
#include <stdio.h>

#include "comparison-test.h"
#include "node/deleting/deleting.h"
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
    char *fileLocation1;
    char *fileLocation2;
    flo_String cssQuery;
    flo_String propToDelete;
    DeletionType deletionType;
    flo_String testName;
} TestFile;

static TestFile testFiles[] = {
    {TEST_FILE_1_BEFORE, TEST_FILE_1_AFTER, FLO_STRING("!DOCTYPE"),
     FLO_STRING("html"), DELETE_BOOLEAN_PROPERTY,
     FLO_STRING("'html' on !DOCTYPE")},
    {TEST_FILE_2_BEFORE, TEST_FILE_2_AFTER, FLO_STRING("input"),
     FLO_STRING("required"), DELETE_BOOLEAN_PROPERTY,
     FLO_STRING("'required' on input")},
    {TEST_FILE_3_BEFORE, TEST_FILE_3_AFTER, FLO_STRING("html"),
     FLO_STRING("lang"), DELETE_PROPERTY, FLO_STRING("'lang' on html")},
};
static ptrdiff_t numTestFiles = FLO_COUNTOF(testFiles);

static void testDeletion(char *fileLocation1, char *fileLocation2,
                         flo_String cssQuery, flo_String propToDelete,
                         DeletionType deletionType, flo_Arena scratch) {
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
    case DELETE_BOOLEAN_PROPERTY: {
        flo_html_removeBooleanProperty(foundNode, propToDelete,
                                       comparisonTest.actual);
        break;
    }
    case DELETE_PROPERTY: {
        flo_html_removeProperty(foundNode, propToDelete, comparisonTest.actual);
        break;
    }
    default: {
        FLO_TEST_FAILURE {
            FLO_ERROR((FLO_STRING("No suitable DeletionType was supplied!\n")));
        }
        return;
    }
    }

    compareAndEndTest(&comparisonTest, scratch);
}

void testNodeDeletions(flo_Arena scratch) {
    FLO_TEST_TOPIC(FLO_STRING("node deletions")) {
        for (ptrdiff_t i = 0; i < numTestFiles; i++) {
            TestFile testFile = testFiles[i];
            FLO_TEST(testFile.testName) {
                testDeletion(testFile.fileLocation1, testFile.fileLocation2,
                             testFile.cssQuery, testFile.propToDelete,
                             testFile.deletionType, scratch);
            }
        }
    }
}
