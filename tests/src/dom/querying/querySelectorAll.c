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
    {TEST_FILE_1, "body div p h1 lalalal input", QUERY_NOT_SEEN_BEFORE, 0,
     "unknown tag"},
    {TEST_FILE_1, "[html-new]", QUERY_NOT_SEEN_BEFORE, 0, "unknown attribute"},
    {TEST_FILE_1, "[html]", QUERY_SUCCESS, 2, "with html attribute"},
    {TEST_FILE_1, "body", QUERY_SUCCESS, 1, "single tag selector"},
    {TEST_FILE_1, "body head", QUERY_SUCCESS, 0, "no nodes found"},
    {TEST_FILE_1, "html[lang=en] > body > div", QUERY_SUCCESS, 7,
     "multiple child tag selector"},
    {TEST_FILE_1, "body div", QUERY_SUCCESS, 8,
     "descendant attribute selector"},
    {TEST_FILE_1, "body [required]", QUERY_SUCCESS, 2,
     "descendant only attribute selector"},
    {TEST_FILE_1, "body>[required]", QUERY_SUCCESS, 1,
     "child only attribute selector"},
    {TEST_FILE_1, "body>[required][a][b][c][d][e][f][g]", QUERY_SUCCESS, 0,
     "maximum filters"},
    {TEST_FILE_1, "body>[required][a][b][c][d][e][f][g][h]",
     QUERY_TOO_MANY_ELEMENT_FILTERS, 0, "1 more than maximum filters"},
    {TEST_FILE_1, "body   >\t\t  [   required]", QUERY_SUCCESS, 1,
     "child only attribute selector, dumb css query"},
    {TEST_FILE_1, "[id=my-first-div] + div", QUERY_SUCCESS, 1,
     "single adjacent sibling"},
    {TEST_FILE_1, "div + div", QUERY_SUCCESS, 5, "multiple adjacent sibling"},
    {TEST_FILE_1, "div ~ div", QUERY_SUCCESS, 6, "general sibling"},
    {TEST_FILE_1, ".big", QUERY_SUCCESS, 4, "using '.' to select by class"},
    {TEST_FILE_1, "p.big", QUERY_SUCCESS, 1,
     "using '.' to select by class after tag selector"},
    {TEST_FILE_1, "div[special-one].big", QUERY_SUCCESS, 1,
     "more difficult query with '.'"},
    {TEST_FILE_1, "#test", QUERY_SUCCESS, 1, "using '#' to select by id"},
    {TEST_FILE_1, "div > div > span,    \t   \tp, title", QUERY_SUCCESS, 7,
     "using ',' to perform multiple queries"},
    {TEST_FILE_1, "body *[special-one]", QUERY_SUCCESS, 2, "using *"},
};

// Calculate the number of test files
static const size_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus testQuery(const char *fileLocation, const char *cssQuery,
                            const flo_html_QueryStatus expectedStatus,
                            const size_t expectedNumberOfNodes) {
    flo_html_TextStore textStore;
    flo_html_ElementStatus initStatus = flo_html_createTextStore(&textStore);
    if (initStatus != ELEMENT_SUCCESS) {
        FLO_HTML_ERROR_WITH_CODE_ONLY(
            flo_html_elementStatusToString(initStatus),
            "Failed to initialize text store");
        return TEST_ERROR_INITIALIZATION;
    }

    flo_html_Dom dom;
    if (createflo_html_DomFromFile(fileLocation, &dom, &textStore) !=
        DOM_SUCCESS) {
        flo_html_destroyTextStore(&textStore);
        return TEST_ERROR_INITIALIZATION;
    }

    flo_html_node_id *results = NULL;
    size_t resultsLen = 0;
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
            for (size_t i = 0; i < resultsLen; i++) {
                printf("%u\n", results[i]);
            }
        }

        printTestDemarcation();
    }

    FLO_HTML_FREE_TO_NULL(results);
    destroyflo_html_Dom(&dom);
    flo_html_destroyTextStore(&textStore);

    return result;
}

unsigned char testQuerySelectorAll(size_t *successes, size_t *failures) {
    printTestTopicStart("querySelectorAll");
    size_t localSuccesses = 0;
    size_t localFailures = 0;

    for (size_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];

        printTestStart(testFile.testName);

        if (testQuery(testFile.fileLocation, testFile.cssQuery,
                      testFile.expectedStatus,
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
