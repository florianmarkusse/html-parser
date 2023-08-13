#include <stdio.h>
#include <stdlib.h>

#include "dom/document-user.h"
#include "dom/document-writing.h"
#include "dom/document.h"
#include "dom/querying/document-querying.h"
#include "querying/querying.h"
#include "test-status.h"
#include "test.h"
#include "type/element/elements.h"

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

    // printDocumentStatus(&doc, &dataContainer);
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
    testAndCount(TEST_FILE_1, "body   >\t\t  [   required]", QUERYING_SUCCESS,
                 1, "child only attribute selector, dumb css query",
                 &localSuccesses, &localFailures);

    printTestScore(localSuccesses, localFailures);

    *successes += localSuccesses;
    *failures += localFailures;

    return localFailures > 0;
}
