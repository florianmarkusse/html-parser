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
    {TEST_FILE_1, FLO_STRING("[html]"), QUERY_SUCCESS, 2,
     FLO_STRING("with html attribute")},
    {TEST_FILE_1, FLO_STRING("body div p h1 lalalal input"),
     QUERY_NOT_SEEN_BEFORE, 0, FLO_STRING("unknown tag")},
    {TEST_FILE_1, FLO_STRING("[html-new]"), QUERY_NOT_SEEN_BEFORE, 0,
     FLO_STRING("unknown attribute")},
    {TEST_FILE_1, FLO_STRING("body"), QUERY_SUCCESS, 1,
     FLO_STRING("single tag selector")},
    {TEST_FILE_1, FLO_STRING("body head"), QUERY_SUCCESS, 0,
     FLO_STRING("no node found")},
    {TEST_FILE_1, FLO_STRING("html[lang=en] > body > div"), QUERY_SUCCESS, 7,
     FLO_STRING("multiple child tag selector")},
    {TEST_FILE_1, FLO_STRING("body div"), QUERY_SUCCESS, 8,
     FLO_STRING("descendant attribute selector")},
    {TEST_FILE_1, FLO_STRING("body [required]"), QUERY_SUCCESS, 2,
     FLO_STRING("descendant only attribute selector")},
    {TEST_FILE_1, FLO_STRING("body>[required]"), QUERY_SUCCESS, 1,
     FLO_STRING("child only attribute selector")},
    {TEST_FILE_1, FLO_STRING("body>[required][a][b][c][d][e][f][g]"),
     QUERY_SUCCESS, 0, FLO_STRING("maximum filters")},
    {TEST_FILE_1, FLO_STRING("body>[required][a][b][c][d][e][f][g][h]"),
     QUERY_TOO_MANY_ELEMENT_FILTERS, 0,
     FLO_STRING("1 more than maximum filters")},
    {TEST_FILE_1, FLO_STRING("body   >\t\t  [   required]"), QUERY_SUCCESS, 1,
     FLO_STRING("child only attribute selector, dumb css query")},
    {TEST_FILE_1, FLO_STRING("[id=my-first-div] + div"), QUERY_SUCCESS, 1,
     FLO_STRING("single adjacent sibling")},
    {TEST_FILE_1, FLO_STRING("div + div"), QUERY_SUCCESS, 5,
     FLO_STRING("multiple adjacent sibling")},
    {TEST_FILE_1, FLO_STRING("div ~ div"), QUERY_SUCCESS, 6,
     FLO_STRING("general sibling")},
    {TEST_FILE_1, FLO_STRING(".big"), QUERY_SUCCESS, 4,
     FLO_STRING("using '.' to select by class")},
    {TEST_FILE_1, FLO_STRING("p.big"), QUERY_SUCCESS, 1,
     FLO_STRING("using '.' to select by class after tag selector")},
    {TEST_FILE_1, FLO_STRING("div[special-one].big"), QUERY_SUCCESS, 1,
     FLO_STRING("more difficult query with '.'")},
    {TEST_FILE_1, FLO_STRING("#test"), QUERY_SUCCESS, 1,
     FLO_STRING("using '#' to select by id")},
    {TEST_FILE_1, FLO_STRING(",,,,,div > div > span, ,,,   \t   \tp, title,,,"),
     QUERY_SUCCESS, 7, FLO_STRING("using ',' to perform multiple queries")},
    {TEST_FILE_1, FLO_STRING("body *[special-one]"), QUERY_SUCCESS, 2,
     FLO_STRING("using *")},
};

// Calculate the number of test files
static ptrdiff_t numTestFiles = FLO_COUNTOF(testFiles);

static void testQuery(char *fileLocation, flo_String cssQuery,
                      flo_html_QueryStatus expectedStatus,
                      ptrdiff_t expectedNumberOfNodes, flo_Arena scratch) {
    flo_html_Dom *dom = flo_html_createDomFromFile(fileLocation, &scratch);
    if (dom == NULL) {
        FLO_TEST_FAILURE {
            FLO_ERROR("Failed to created DOM from file ");
            FLO_ERROR(fileLocation, FLO_NEWLINE);
        }
        return;
    }

    flo_html_node_id_a results;
    flo_html_QueryStatus actual =
        flo_html_querySelectorAll(cssQuery, dom, &results, &scratch);

    if (actual == expectedStatus && (expectedStatus != QUERY_SUCCESS ||
                                     results.len == expectedNumberOfNodes)) {
        flo_testSuccess();
    } else {
        FLO_TEST_FAILURE {
            if (actual != expectedStatus) {
                flo_appendExpectCodeWithString(
                    expectedStatus,
                    flo_html_queryingStatusToString(expectedStatus), actual,
                    flo_html_queryingStatusToString(actual));
            } else {
                flo_appendExpectUint(expectedNumberOfNodes, results.len);
                FLO_ERROR((FLO_STRING("Node IDs received...\n")));
                for (ptrdiff_t i = 0; i < results.len; i++) {
                    FLO_ERROR(results.buf[i], FLO_NEWLINE);
                }
            }
        }
    }
}

void testQuerySelectorAll(flo_Arena scratch){
    FLO_TEST_TOPIC(FLO_STRING("querySelectorAll")){

        for (ptrdiff_t i = 0; i < numTestFiles;
             i++){TestFile testFile = testFiles[i];

FLO_TEST(testFile.testName) {
    testQuery(testFile.fileLocation, testFile.cssQuery, testFile.expectedStatus,
              testFile.expectedResult, scratch);
}
}
}
}
;
