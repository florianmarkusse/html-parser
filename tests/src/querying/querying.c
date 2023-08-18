#include <dom/document-user.h>
#include <dom/document-writing.h>
#include <dom/document.h>
#include <dom/querying/document-querying.h>
#include <stdio.h>
#include <stdlib.h>
#include <type/element/elements.h>

#include "querying/querying.h"
#include "test-status.h"
#include "test.h"

#define CURRENT_DIR "tests/src/querying/inputs/"
#define TEST_FILE_1 CURRENT_DIR "test-1.html"

TestStatus testQuery(const char *fileLocation, const char *cssQuery,
                     const QueryingStatus expectedStatus,
                     const size_t expectedNumberOfNodes) {
    DataContainer dataContainer;
    createDataContainer(&dataContainer);

    Document doc;
    if (createFromFile(fileLocation, &doc, &dataContainer) !=
        DOCUMENT_SUCCESS) {
        destroyDataContainer(&dataContainer);
        return TEST_ERROR_INITIALIZATION;
    }

    node_id *results = NULL;
    size_t resultsLen = 0;
    QueryingStatus actual =
        querySelectorAll(cssQuery, &doc, &dataContainer, &results, &resultsLen);

    TestStatus result = TEST_FAILURE;

    if (actual == expectedStatus && (expectedStatus != QUERYING_SUCCESS ||
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
            printTestResultDifferenceNumber(
                expectedStatus, expectedNumberOfNodes, actual, resultsLen);

            printf("Node IDs received...\n");
            for (size_t i = 0; i < resultsLen; i++) {
                printf("%u\n", results[i]);
            }
        }

        printTestDemarcation();
    }

    free(results);
    destroyDocument(&doc);
    destroyDataContainer(&dataContainer);

    return result;
}

static inline void testBadInit(size_t *localSuccsses, size_t *localFailures) {
    printTestStart("bad init");

    DataContainer dataContainer;
    Document doc;
    node_id *results = NULL;
    size_t resultsLen = 1;
    QueryingStatus wrongLenStatus =
        querySelectorAll("", &doc, &dataContainer, &results, &resultsLen);
    resultsLen = 0;
    node_id *withVals = malloc(sizeof(node_id));
    QueryingStatus wrongArrayStatus =
        querySelectorAll("", &doc, &dataContainer, &withVals, &resultsLen);
    free(withVals);

    if (wrongLenStatus == QUERYING_INITIALIZATION_ERROR &&
        wrongArrayStatus == QUERYING_INITIALIZATION_ERROR) {
        (*localSuccsses)++;
        printTestSuccess();
    } else {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceString(
            QUERYING_INITIALIZATION_ERROR,
            queryingStatusToString(QUERYING_INITIALIZATION_ERROR),
            wrongLenStatus, queryingStatusToString(wrongLenStatus));
        printTestResultDifferenceString(
            QUERYING_INITIALIZATION_ERROR,
            queryingStatusToString(QUERYING_INITIALIZATION_ERROR),
            wrongArrayStatus, queryingStatusToString(wrongArrayStatus));
        printTestDemarcation();
        (*localFailures)++;
    }
}

static inline void testAndCount(const char *fileLocation, const char *cssQuery,
                                const QueryingStatus expectedStatus,
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

unsigned char testQueries(size_t *successes, size_t *failures) {
    printTestTopicStart("document queries");
    size_t localSuccesses = 0;
    size_t localFailures = 0;

    testBadInit(&localSuccesses, &localFailures);
    testAndCount(TEST_FILE_1, "body div p h1 lalalal input", QUERYING_NOT_FOUND,
                 0, "unkown tag", &localSuccesses, &localFailures);
    testAndCount(TEST_FILE_1, "[html-new]", QUERYING_NOT_FOUND, 0,
                 "unknown attribute", &localSuccesses, &localFailures);
    testAndCount(TEST_FILE_1, "[html]", QUERYING_SUCCESS, 2,
                 "with html attribute", &localSuccesses, &localFailures);
    testAndCount(TEST_FILE_1, "body", QUERYING_SUCCESS, 1,
                 "single tag selector", &localSuccesses, &localFailures);
    testAndCount(TEST_FILE_1, "body head", QUERYING_SUCCESS, 0,
                 "no nodes found", &localSuccesses, &localFailures);
    testAndCount(TEST_FILE_1, "html[lang=en] > body > div", QUERYING_SUCCESS, 7,
                 "multiple child tag selector", &localSuccesses,
                 &localFailures);
    testAndCount(TEST_FILE_1, "body div", QUERYING_SUCCESS, 8,
                 "descendant attribute selector", &localSuccesses,
                 &localFailures);
    testAndCount(TEST_FILE_1, "body [required]", QUERYING_SUCCESS, 2,
                 "descendant only attribute selector", &localSuccesses,
                 &localFailures);
    testAndCount(TEST_FILE_1, "body>[required]", QUERYING_SUCCESS, 1,
                 "child only attribute selector", &localSuccesses,
                 &localFailures);
    testAndCount(TEST_FILE_1, "body>[required][a][b][c][d][e][f][g]",
                 QUERYING_SUCCESS, 0, "maximum filters", &localSuccesses,
                 &localFailures);
    testAndCount(TEST_FILE_1, "body>[required][a][b][c][d][e][f][g][h]",
                 QUERYING_TOO_MANY_ELEMENT_FILTERS, 0,
                 "1 more than maximum filters", &localSuccesses,
                 &localFailures);
    testAndCount(TEST_FILE_1, "body   >\t\t  [   required]", QUERYING_SUCCESS,
                 1, "child only attribute selector, dumb css query",
                 &localSuccesses, &localFailures);
    testAndCount(TEST_FILE_1, "[id=my-first-div] + div", QUERYING_SUCCESS, 1,
                 "single adjacent sibling", &localSuccesses, &localFailures);
    testAndCount(TEST_FILE_1, "div + div", QUERYING_SUCCESS, 5,
                 "multiple adjacent sibling", &localSuccesses, &localFailures);
    testAndCount(TEST_FILE_1, "div ~ div", QUERYING_SUCCESS, 6,
                 "general sibling", &localSuccesses, &localFailures);

    printTestScore(localSuccesses, localFailures);

    *successes += localSuccesses;
    *failures += localFailures;

    return localFailures > 0;
}
