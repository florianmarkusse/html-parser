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

static const TestFile testFiles[] = {
    {TEST_FILE_1, "body div p h1 lalalal input", QUERY_NOT_SEEN_BEFORE, 0,
     "unknown tag"},
    {TEST_FILE_1, "[html-new]", QUERY_NOT_SEEN_BEFORE, 0, "unknown attribute"},
    {TEST_FILE_1, "[html]", QUERY_SUCCESS, 1, "with html attribute"},
    {TEST_FILE_1, "body", QUERY_SUCCESS, 6, "single tag selector"},
    {TEST_FILE_1, "body head", QUERY_SUCCESS, 0, "no nodes found"},
};

// Calculate the number of test files
static const size_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus testQuery(const char *fileLocation, const char *cssQuery,
                            const QueryStatus expectedStatus,
                            const node_id expectedNode) {
    DataContainer dataContainer;
    createDataContainer(&dataContainer);

    Dom dom;
    if (createFromFile(fileLocation, &dom, &dataContainer) != DOM_SUCCESS) {
        destroyDataContainer(&dataContainer);
        return TEST_ERROR_INITIALIZATION;
    }

    node_id actualNode = 0;
    QueryStatus actual =
        querySelector(cssQuery, &dom, &dataContainer, &actualNode);

    TestStatus result = TEST_FAILURE;

    if (actual == expectedStatus &&
        (expectedStatus != QUERY_SUCCESS || actualNode == expectedNode)) {
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
            printTestResultDifferenceNumber(expectedNode, actualNode);
        }

        printTestDemarcation();
    }

    destroyDom(&dom);
    destroyDataContainer(&dataContainer);

    return result;
}

static inline void testAndCount(const char *fileLocation, const char *cssQuery,
                                const QueryStatus expectedStatus,
                                const node_id expectedNode,
                                const char *testName, size_t *localSuccsses,
                                size_t *localFailures) {
    printTestStart(testName);

    if (testQuery(fileLocation, cssQuery, expectedStatus, expectedNode) ==
        TEST_SUCCESS) {
        (*localSuccsses)++;
    } else {
        (*localFailures)++;
    }
}

unsigned char testQuerySelector(size_t *successes, size_t *failures) {
    printTestTopicStart("querySelector");
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
}
