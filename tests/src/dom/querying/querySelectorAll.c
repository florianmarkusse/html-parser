#include <flo/html-parser/dom/dom-user.h>
#include <flo/html-parser/dom/dom-writing.h>
#include <flo/html-parser/dom/dom.h>
#include <flo/html-parser/dom/query/dom-query.h>
#include <flo/html-parser/type/element/elements.h>
#include <flo/html-parser/utils/memory/memory.h>
#include <flo/html-parser/utils/print/error.h>
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

static TestStatus testQuery(const char *fileLocation, char *cssQuery,
                            const QueryStatus expectedStatus,
                            const size_t expectedNumberOfNodes) {
    DataContainer dataContainer;
    ElementStatus initStatus = createDataContainer(&dataContainer);
    if (initStatus != ELEMENT_SUCCESS) {
        ERROR_WITH_CODE_ONLY(elementStatusToString(initStatus),
                             "Failed to initialize data container");
        return TEST_ERROR_INITIALIZATION;
    }

    Dom dom;
    if (createFromFile(fileLocation, &dom, &dataContainer) != DOM_SUCCESS) {
        destroyDataContainer(&dataContainer);
        return TEST_ERROR_INITIALIZATION;
    }

    //    printDomStatus(&dom, &dataContainer);
    //    printAttributeStatus(&dataContainer);

    node_id *results = NULL;
    size_t resultsLen = 0;
    QueryStatus actual =
        querySelectorAll(cssQuery, &dom, &dataContainer, &results, &resultsLen);

    TestStatus result = TEST_FAILURE;

    if (actual == expectedStatus && (expectedStatus != QUERY_SUCCESS ||
                                     resultsLen == expectedNumberOfNodes)) {
        printTestSuccess();
        result = TEST_SUCCESS;
    } else {
        printTestFailure();
        printTestDemarcation();
        if (actual != expectedStatus) {
            printTestResultDifferenceString(
                expectedStatus, queryingStatusToString(expectedStatus), actual,
                queryingStatusToString(actual));
        } else {
            printTestResultDifferenceNumber(expectedNumberOfNodes, resultsLen);
            printf("Node IDs received...\n");
            for (size_t i = 0; i < resultsLen; i++) {
                printf("%u\n", results[i]);
            }
        }

        printTestDemarcation();
    }

    FREE_TO_NULL(results);
    destroyDom(&dom);
    destroyDataContainer(&dataContainer);

    return result;
}

static inline void testAndCount(const char *fileLocation, char *cssQuery,
                                const QueryStatus expectedStatus,
                                const size_t expectedNumberOfNodes,
                                const char *testName, size_t *localSuccsses,
                                size_t *localFailures) {
    printTestStart(testName);

    if (testQuery(fileLocation, cssQuery, expectedStatus,
                  expectedNumberOfNodes) == TEST_SUCCESS) {
        (*localSuccsses)++;
    } else {
        (*localFailures)++;
    }
}

unsigned char testQuerySelectorAll(size_t *successes, size_t *failures) {
    printTestTopicStart("querySelectorAll");
    size_t localSuccesses = 0;
    size_t localFailures = 0;

    for (size_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];
        testAndCount(testFile.fileLocation, testFile.cssQuery,
                     testFile.expectedStatus, testFile.expectedResult,
                     testFile.testName, &localSuccesses, &localFailures);
    }

    printTestScore(localSuccesses, localFailures);
    *successes += localSuccesses;
    *failures += localFailures;

    return localFailures > 0;
};
;
