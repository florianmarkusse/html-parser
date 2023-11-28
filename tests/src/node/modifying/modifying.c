#include <flo/html-parser.h>
#include <stdbool.h>
#include <stdio.h>

#include "comparison-test.h"
#include "node/modifying/modifying.h"
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
    flo_String cssQuery;
    flo_String propKey;
    flo_String newPropValue;
    flo_String testName;
} TestFile;

static TestFile testFiles[] = {
    {TEST_FILE_1_BEFORE, TEST_FILE_1_AFTER, FLO_STRING("body"),
     FLO_STRING("style"), FLO_STRING("newstyle"),
     FLO_STRING("change property value")},
    {TEST_FILE_2_BEFORE, TEST_FILE_2_AFTER, FLO_STRING("#text-content-test"),
     FLO_STRING("id"), FLO_STRING("id-changed"), FLO_STRING("change id value")},
    {TEST_FILE_3_BEFORE, TEST_FILE_3_AFTER, FLO_STRING("#text-content-test"),
     FLO_STRING("I am the new text content, bow for me!"), FLO_EMPTY_STRING,
     FLO_STRING("setting text content")},
};
static ptrdiff_t numTestFiles = FLO_COUNTOF(testFiles);

static void testModification(char *fileLocation1, char *fileLocation2,
                             flo_String cssQuery, flo_String propKey,
                             flo_String newPropValue, flo_Arena scratch) {
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

    if (newPropValue.len > 0) {
        if (!flo_html_setPropertyValue(foundNode, propKey, newPropValue,
                                       comparisonTest.actual, &scratch)) {
            FLO_TEST_FAILURE {
                FLO_ERROR((FLO_STRING("Failed to set property value!\n")));
            }
            return;
        }
    } else {
        flo_html_setTextContent(foundNode, propKey, comparisonTest.actual,
                                &scratch);
    }

    compareAndEndTest(&comparisonTest, scratch);
}

void testNodeModifications(flo_Arena scratch) {
    FLO_TEST_TOPIC(FLO_STRING("node modifications")) {
        for (ptrdiff_t i = 0; i < numTestFiles; i++) {
            TestFile testFile = testFiles[i];

            FLO_TEST(testFile.testName) {
                testModification(testFile.fileLocation1, testFile.fileLocation2,
                                 testFile.cssQuery, testFile.propKey,
                                 testFile.newPropValue, scratch);
            }
        }
    }
}
