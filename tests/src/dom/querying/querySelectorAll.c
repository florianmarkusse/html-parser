#include <flo/html-parser.h>
#include <flo/html-parser/utils/memory/memory.h>
#include <stdio.h>
#include <stdlib.h>

#include "dom/querying/querying.h"
#include "test-status.h"
#include "test.h"

#define CURRENT_DIR "tests/src/dom/querying/inputs/"
#define TEST_FILE_1 CURRENT_DIR "test-1.html"

static const TestFile testFiles[] = {
    {TEST_FILE_1, FLO_HTML_S("[html]"), QUERY_SUCCESS, 2,
     "with html attribute"},
    {TEST_FILE_1, FLO_HTML_S("body div p h1 lalalal input"),
     QUERY_NOT_SEEN_BEFORE, 0, "unknown tag"},
    {TEST_FILE_1, FLO_HTML_S("[html-new]"), QUERY_NOT_SEEN_BEFORE, 0,
     "unknown attribute"},
    {TEST_FILE_1, FLO_HTML_S("body"), QUERY_SUCCESS, 1, "single tag selector"},
    {TEST_FILE_1, FLO_HTML_S("body head"), QUERY_SUCCESS, 0, "no nodes found"},
    {TEST_FILE_1, FLO_HTML_S("html[lang=en] > body > div"), QUERY_SUCCESS, 7,
     "multiple child tag selector"},
    {TEST_FILE_1, FLO_HTML_S("body div"), QUERY_SUCCESS, 8,
     "descendant attribute selector"},
    {TEST_FILE_1, FLO_HTML_S("body [required]"), QUERY_SUCCESS, 2,
     "descendant only attribute selector"},
    {TEST_FILE_1, FLO_HTML_S("body>[required]"), QUERY_SUCCESS, 1,
     "child only attribute selector"},
    {TEST_FILE_1, FLO_HTML_S("body>[required][a][b][c][d][e][f][g]"),
     QUERY_SUCCESS, 0, "maximum filters"},
    {TEST_FILE_1, FLO_HTML_S("body>[required][a][b][c][d][e][f][g][h]"),
     QUERY_TOO_MANY_ELEMENT_FILTERS, 0, "1 more than maximum filters"},
    {TEST_FILE_1, FLO_HTML_S("body   >\t\t  [   required]"), QUERY_SUCCESS, 1,
     "child only attribute selector, dumb css query"},
    {TEST_FILE_1, FLO_HTML_S("[id=my-first-div] + div"), QUERY_SUCCESS, 1,
     "single adjacent sibling"},
    {TEST_FILE_1, FLO_HTML_S("div + div"), QUERY_SUCCESS, 5,
     "multiple adjacent sibling"},
    {TEST_FILE_1, FLO_HTML_S("div ~ div"), QUERY_SUCCESS, 6, "general sibling"},
    {TEST_FILE_1, FLO_HTML_S(".big"), QUERY_SUCCESS, 4,
     "using '.' to select by class"},
    {TEST_FILE_1, FLO_HTML_S("p.big"), QUERY_SUCCESS, 1,
     "using '.' to select by class after tag selector"},
    {TEST_FILE_1, FLO_HTML_S("div[special-one].big"), QUERY_SUCCESS, 1,
     "more difficult query with '.'"},
    {TEST_FILE_1, FLO_HTML_S("#test"), QUERY_SUCCESS, 1,
     "using '#' to select by id"},
    {TEST_FILE_1, FLO_HTML_S("div > div > span,    \t   \tp, title"),
     QUERY_SUCCESS, 7, "using ',' to perform multiple queries"},
    {TEST_FILE_1, FLO_HTML_S("body *[special-one]"), QUERY_SUCCESS, 2,
     "using *"},
};

// Calculate the number of test files
static const ptrdiff_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus testQuery(const flo_html_String fileLocation,
                            const flo_html_String cssQuery,
                            const flo_html_QueryStatus expectedStatus,
                            const ptrdiff_t expectedNumberOfNodes) {
    flo_html_TextStore textStore;
    flo_html_ElementStatus initStatus = flo_html_createTextStore(&textStore);
    if (initStatus != ELEMENT_SUCCESS) {
        FLO_HTML_ERROR_WITH_CODE_ONLY(
            flo_html_elementStatusToString(initStatus),
            "Failed to initialize text store");
        return TEST_ERROR_INITIALIZATION;
    }

    flo_html_Dom dom;
    if (flo_html_createDomFromFile(fileLocation, &dom, &textStore) !=
        DOM_SUCCESS) {
        flo_html_destroyTextStore(&textStore);
        return TEST_ERROR_INITIALIZATION;
    }

    flo_html_node_id *results = NULL;
    ptrdiff_t resultsLen = 0;
    flo_html_QueryStatus actual = flo_html_querySelectorAll(
        cssQuery, &dom, &textStore, &results, &resultsLen);

    TestStatus result = TEST_FAILURE;

    if (actual == expectedStatus && (expectedStatus != QUERY_SUCCESS ||
                                     resultsLen == expectedNumberOfNodes)) {
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
            printTestResultDifferenceNumber(expectedNumberOfNodes, resultsLen);
            printf("Node IDs received...\n");
            for (ptrdiff_t i = 0; i < resultsLen; i++) {
                printf("%u\n", results[i]);
            }
        }

        printTestDemarcation();
    }

    FLO_HTML_FREE_TO_NULL(results);
    flo_html_destroyDom(&dom);
    flo_html_destroyTextStore(&textStore);

    return result;
}

unsigned char testQuerySelectorAll(ptrdiff_t *successes, ptrdiff_t *failures) {
    printTestTopicStart("querySelectorAll");
    ptrdiff_t localSuccesses = 0;
    ptrdiff_t localFailures = 0;

    for (ptrdiff_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];

        printTestStart(testFile.testName);

        if (testQuery(FLO_HTML_S_LEN(testFile.fileLocation,
                                     strlen(testFile.fileLocation)),
                      testFile.cssQuery, testFile.expectedStatus,
                      testFile.expectedResult) != TEST_SUCCESS) {
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
