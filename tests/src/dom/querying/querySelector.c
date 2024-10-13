#include "flo/util/log.h"
#include <flo/html-parser.h>
#include <memory/arena.h>
#include <stdio.h>
#include <stdlib.h>

#include "dom/querying/querying.h"
#include "expectations.h"
#include "test.h"

#define CURRENT_DIR "tests/src/dom/querying/inputs/"
#define TEST_FILE_1 CURRENT_DIR "test-1.html"

static TestFile testFiles[] = {
    {TEST_FILE_1, FLO_STRING("body div p h1 lalalal input"),
     QUERY_NOT_SEEN_BEFORE, 0, FLO_STRING("unknown tag")},
    {TEST_FILE_1, FLO_STRING("[html-new]"), QUERY_NOT_SEEN_BEFORE, 0,
     FLO_STRING("unknown attribute")},
    {TEST_FILE_1, FLO_STRING("[html]"), QUERY_SUCCESS, 2,
     FLO_STRING("with html attribute")},
    {TEST_FILE_1, FLO_STRING("body"), QUERY_SUCCESS, 7,
     FLO_STRING("single tag selector")},
    {TEST_FILE_1, FLO_STRING("body head"), QUERY_SUCCESS, 0,
     FLO_STRING("no nodes found")},
};

static ptrdiff_t numTestFiles = FLO_COUNTOF(testFiles);

static void testQuery(char *fileLocation, flo_String cssQuery,
                      flo_html_QueryStatus expectedStatus,
                      flo_html_node_id expectedNode, flo_Arena scratch) {
    flo_html_Dom *dom = flo_html_createDomFromFile(fileLocation, &scratch);
    if (dom == NULL) {
        FLO_TEST_FAILURE {
            FLO_ERROR("Failed to created DOM from file ");
            FLO_ERROR(fileLocation, FLO_NEWLINE);
        }
        return;
    }

    flo_html_node_id actualNode = 0;
    flo_html_QueryStatus actual =
        flo_html_querySelector(cssQuery, dom, &actualNode, scratch);

    if (actual == expectedStatus &&
        (expectedStatus != QUERY_SUCCESS || actualNode == expectedNode)) {
        flo_testSuccess();
    } else {
        FLO_TEST_FAILURE {
            if (actual != expectedStatus) {
                flo_appendExpectCodeWithString(
                    expectedStatus,
                    flo_html_queryingStatusToString(expectedStatus), actual,
                    flo_html_queryingStatusToString(actual));
            } else {
                flo_appendExpectUint(expectedNode, actualNode);
            }
        }
    }
}

void testQuerySelector(flo_Arena scratch) {
    FLO_TEST_TOPIC(FLO_STRING("querySelector")) {
        for (ptrdiff_t i = 0; i < numTestFiles; i++) {
            TestFile testFile = testFiles[i];

            FLO_TEST(testFile.testName) {
                testQuery(testFile.fileLocation, testFile.cssQuery,
                          testFile.expectedStatus,
                          (flo_html_node_id)testFile.expectedResult, scratch);
            }
        }
    }
}
