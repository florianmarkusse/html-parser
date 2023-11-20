#include <flo/html-parser.h>
#include <memory/arena.h>
#include <stdio.h>
#include <stdlib.h>

#include "dom/querying/querying.h"
#include "test-status.h"
#include "test.h"

#define CURRENT_DIR "tests/src/dom/querying/inputs/"
#define TEST_FILE_1 CURRENT_DIR "test-1.html"

static TestFile testFiles[] = {
    {TEST_FILE_1, FLO_STRING("[html]"), QUERY_SUCCESS, 2,
     "with html attribute"},
    {TEST_FILE_1, FLO_STRING("body div p h1 lalalal input"),
     QUERY_NOT_SEEN_BEFORE, 0, "unknown tag"},
    {TEST_FILE_1, FLO_STRING("[html-new]"), QUERY_NOT_SEEN_BEFORE, 0,
     "unknown attribute"},
    {TEST_FILE_1, FLO_STRING("body"), QUERY_SUCCESS, 1, "single tag selector"},
    {TEST_FILE_1, FLO_STRING("body head"), QUERY_SUCCESS, 0, "no node found"},
    {TEST_FILE_1, FLO_STRING("html[lang=en] > body > div"), QUERY_SUCCESS, 7,
     "multiple child tag selector"},
    {TEST_FILE_1, FLO_STRING("body div"), QUERY_SUCCESS, 8,
     "descendant attribute selector"},
    {TEST_FILE_1, FLO_STRING("body [required]"), QUERY_SUCCESS, 2,
     "descendant only attribute selector"},
    {TEST_FILE_1, FLO_STRING("body>[required]"), QUERY_SUCCESS, 1,
     "child only attribute selector"},
    {TEST_FILE_1, FLO_STRING("body>[required][a][b][c][d][e][f][g]"),
     QUERY_SUCCESS, 0, "maximum filters"},
    {TEST_FILE_1, FLO_STRING("body>[required][a][b][c][d][e][f][g][h]"),
     QUERY_TOO_MANY_ELEMENT_FILTERS, 0, "1 more than maximum filters"},
    {TEST_FILE_1, FLO_STRING("body   >\t\t  [   required]"), QUERY_SUCCESS, 1,
     "child only attribute selector, dumb css query"},
    {TEST_FILE_1, FLO_STRING("[id=my-first-div] + div"), QUERY_SUCCESS, 1,
     "single adjacent sibling"},
    {TEST_FILE_1, FLO_STRING("div + div"), QUERY_SUCCESS, 5,
     "multiple adjacent sibling"},
    {TEST_FILE_1, FLO_STRING("div ~ div"), QUERY_SUCCESS, 6, "general sibling"},
    {TEST_FILE_1, FLO_STRING(".big"), QUERY_SUCCESS, 4,
     "using '.' to select by class"},
    {TEST_FILE_1, FLO_STRING("p.big"), QUERY_SUCCESS, 1,
     "using '.' to select by class after tag selector"},
    {TEST_FILE_1, FLO_STRING("div[special-one].big"), QUERY_SUCCESS, 1,
     "more difficult query with '.'"},
    {TEST_FILE_1, FLO_STRING("#test"), QUERY_SUCCESS, 1,
     "using '#' to select by id"},
    {TEST_FILE_1, FLO_STRING(",,,,,div > div > span, ,,,   \t   \tp, title,,,"),
     QUERY_SUCCESS, 7, "using ',' to perform multiple queries"},
    {TEST_FILE_1, FLO_STRING("body *[special-one]"), QUERY_SUCCESS, 2,
     "using *"},
};

// Calculate the number of test files
static ptrdiff_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus testQuery(flo_String fileLocation, flo_String cssQuery,
                            flo_html_QueryStatus expectedStatus,
                            ptrdiff_t expectedNumberOfNodes,
                            flo_Arena scratch) {
    flo_html_Dom *dom = flo_html_createDomFromFile(fileLocation, &scratch);
    if (dom == NULL) {
        FLO_PRINT_ERROR("Failed to created DOM from file %.*s\n",
                        FLO_STRING_PRINT(fileLocation));
        return TEST_ERROR_INITIALIZATION;
    }

    flo_html_node_id_a results;
    flo_html_QueryStatus actual =
        flo_html_querySelectorAll(cssQuery, dom, &results, &scratch);

    TestStatus result = TEST_FAILURE;

    if (actual == expectedStatus && (expectedStatus != QUERY_SUCCESS ||
                                     results.len == expectedNumberOfNodes)) {
        printTestSuccess();
        result = TEST_SUCCESS;
    } else {
        printTestFailure();
        printTestDemarcation();
        if (actual != expectedStatus) {
            printTestResultDifferenceErrorCode(
                expectedStatus, flo_html_queryingStatusToString(expectedStatus),
                actual, flo_html_queryingStatusToString(actual));
        } else {
            printTestResultDifferenceNumber(expectedNumberOfNodes, results.len);
            printf("Node IDs received...\n");
            for (ptrdiff_t i = 0; i < results.len; i++) {
                printf("%u\n", results.buf[i]);
            }
        }

        printTestDemarcation();
    }

    return result;
}

unsigned char testQuerySelectorAll(ptrdiff_t *successes, ptrdiff_t *failures,
                                   flo_Arena scratch) {
    printTestTopicStart("querySelectorAll");
    ptrdiff_t localSuccesses = 0;
    ptrdiff_t localFailures = 0;

    for (ptrdiff_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];

        printTestStart(testFile.testName);

        if (testQuery(FLO_STRING_LEN(testFile.fileLocation,
                                     strlen(testFile.fileLocation)),
                      testFile.cssQuery, testFile.expectedStatus,
                      testFile.expectedResult, scratch) != TEST_SUCCESS) {
            localFailures++;
        } else {
            localSuccesses++;
        }
    }

    printTestScore(localSuccesses, localFailures);
    *successes += localSuccesses;
    *failures += localFailures;

    return localFailures > 0;
};
