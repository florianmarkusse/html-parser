#include <flo/html-parser/dom/dom-user.h>
#include <flo/html-parser/dom/dom-writing.h>
#include <flo/html-parser/dom/dom.h>
#include <flo/html-parser/dom/query/dom-query.h>
#include <flo/html-parser/type/element/elements.h>
#include <stdio.h>
#include <stdlib.h>

#include "querying/querying.h"
#include "test-status.h"
#include "test.h"

#define CURRENT_DIR "tests/src/querying/inputs/"
#define TEST_FILE_1 CURRENT_DIR "test-1.html"

static TestStatus testQuery(const char *fileLocation, const char *cssQuery,
                            const QueryStatus expectedStatus,
                            const size_t expectedNumberOfNodes) {
    DataContainer dataContainer;
    createDataContainer(&dataContainer);

    Dom dom;
    if (createFromFile(fileLocation, &dom, &dataContainer) != DOM_SUCCESS) {
        destroyDataContainer(&dataContainer);
        return TEST_ERROR_INITIALIZATION;
    }

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

    free(results);
    destroyDom(&dom);
    destroyDataContainer(&dataContainer);

    return result;
}

static void testBadInit(size_t *localSuccsses, size_t *localFailures) {
    printTestStart("bad init");

    DataContainer dataContainer;
    Dom dom;
    node_id *results = NULL;
    size_t resultsLen = 1;
    QueryStatus wrongLenStatus =
        querySelectorAll("", &dom, &dataContainer, &results, &resultsLen);
    resultsLen = 0;
    node_id *withVals = malloc(sizeof(node_id));
    QueryStatus wrongArrayStatus =
        querySelectorAll("", &dom, &dataContainer, &withVals, &resultsLen);
    free(withVals);

    if (wrongLenStatus == QUERY_INITIALIZATION_ERROR &&
        wrongArrayStatus == QUERY_INITIALIZATION_ERROR) {
        (*localSuccsses)++;
        printTestSuccess();
    } else {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceString(
            QUERY_INITIALIZATION_ERROR,
            queryingStatusToString(QUERY_INITIALIZATION_ERROR), wrongLenStatus,
            queryingStatusToString(wrongLenStatus));
        printTestResultDifferenceString(
            QUERY_INITIALIZATION_ERROR,
            queryingStatusToString(QUERY_INITIALIZATION_ERROR),
            wrongArrayStatus, queryingStatusToString(wrongArrayStatus));
        printTestDemarcation();
        (*localFailures)++;
    }
}

static inline void testAndCount(const char *fileLocation, const char *cssQuery,
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

    testBadInit(&localSuccesses, &localFailures);
    testAndCount(TEST_FILE_1, "body div p h1 lalalal input",
                 QUERY_NOT_SEEN_BEFORE, 0, "unkown tag", &localSuccesses,
                 &localFailures);
    testAndCount(TEST_FILE_1, "[html-new]", QUERY_NOT_SEEN_BEFORE, 0,
                 "unknown attribute", &localSuccesses, &localFailures);
    testAndCount(TEST_FILE_1, "[html]", QUERY_SUCCESS, 2, "with html attribute",
                 &localSuccesses, &localFailures);
    testAndCount(TEST_FILE_1, "body", QUERY_SUCCESS, 1, "single tag selector",
                 &localSuccesses, &localFailures);
    testAndCount(TEST_FILE_1, "body head", QUERY_SUCCESS, 0, "no nodes found",
                 &localSuccesses, &localFailures);
    testAndCount(TEST_FILE_1, "html[lang=en] > body > div", QUERY_SUCCESS, 7,
                 "multiple child tag selector", &localSuccesses,
                 &localFailures);
    testAndCount(TEST_FILE_1, "body div", QUERY_SUCCESS, 8,
                 "descendant attribute selector", &localSuccesses,
                 &localFailures);
    testAndCount(TEST_FILE_1, "body [required]", QUERY_SUCCESS, 2,
                 "descendant only attribute selector", &localSuccesses,
                 &localFailures);
    testAndCount(TEST_FILE_1, "body>[required]", QUERY_SUCCESS, 1,
                 "child only attribute selector", &localSuccesses,
                 &localFailures);
    testAndCount(TEST_FILE_1, "body>[required][a][b][c][d][e][f][g]",
                 QUERY_SUCCESS, 0, "maximum filters", &localSuccesses,
                 &localFailures);
    testAndCount(TEST_FILE_1, "body>[required][a][b][c][d][e][f][g][h]",
                 QUERY_TOO_MANY_ELEMENT_FILTERS, 0,
                 "1 more than maximum filters", &localSuccesses,
                 &localFailures);
    testAndCount(TEST_FILE_1, "body   >\t\t  [   required]", QUERY_SUCCESS, 1,
                 "child only attribute selector, dumb css query",
                 &localSuccesses, &localFailures);
    testAndCount(TEST_FILE_1, "[id=my-first-div] + div", QUERY_SUCCESS, 1,
                 "single adjacent sibling", &localSuccesses, &localFailures);
    testAndCount(TEST_FILE_1, "div + div", QUERY_SUCCESS, 5,
                 "multiple adjacent sibling", &localSuccesses, &localFailures);
    testAndCount(TEST_FILE_1, "div ~ div", QUERY_SUCCESS, 6, "general sibling",
                 &localSuccesses, &localFailures);

    printTestScore(localSuccesses, localFailures);

    *successes += localSuccesses;
    *failures += localFailures;

    return localFailures > 0;
}
