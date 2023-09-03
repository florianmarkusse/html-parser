#include <flo/html-parser/dom/query/query.h>
#include <flo/html-parser/dom/reading/reading.h>
#include <flo/html-parser/dom/user.h>
#include <flo/html-parser/utils/memory/memory.h>
#include <flo/html-parser/utils/print/error.h>
#include <stdio.h>
#include <string.h>

#include "node/querying/querying.h"
#include "test-status.h"
#include "test.h"

typedef enum { TEXT_CONTENT, NUM_CHAR_FUNCTION_TYPES } ArrayFunctionType;

#define CURRENT_DIR "tests/src/node/querying/inputs/"
#define TEST_FILE_1 CURRENT_DIR "test-1.html"

typedef struct {
    const char *fileLocation;
    const char *cssQuery;
    const size_t expectedResult;
    const ArrayFunctionType functionType;
    const char *testName;
} __attribute__((aligned(64))) TestFile;

static const TestFile testFiles[] = {
    {TEST_FILE_1, "#my-first-div", 1, TEXT_CONTENT, "nested text node"},
    {TEST_FILE_1, "title", 1, TEXT_CONTENT, "simple text node"},
    {TEST_FILE_1, "span > p", 0, TEXT_CONTENT, "no text nodes"},
    {TEST_FILE_1, "#text-content-test", 5, TEXT_CONTENT,
     "multiple text nodes 1"},
    {TEST_FILE_1, "body", 7, TEXT_CONTENT, "multiple text nodes 2"},
};

static const size_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus testQuery(const char *fileLocation, const char *cssQuery,
                            const ArrayFunctionType functionType,
                            const size_t expectedResult) {
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

    TestStatus result = TEST_FAILURE;
    node_id foundNode = 0;
    QueryStatus queryStatus =
        querySelector(cssQuery, &dom, &dataContainer, &foundNode);

    if (queryStatus != QUERY_SUCCESS) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceErrorCode(
            QUERY_SUCCESS, queryingStatusToString(QUERY_SUCCESS), queryStatus,
            queryingStatusToString(queryStatus));
        printTestDemarcation();
    } else {
        size_t actualResult = 0;
        switch (functionType) {
        case TEXT_CONTENT: {
            const char **results = NULL;
            queryStatus =
                getTextContent(foundNode, &dom, &results, &actualResult);
            FREE_TO_NULL(results);
            break;
        }
        default: {
            printTestFailure();
            printTestDemarcation();
            printf("No suitable enum was supplied!\n");
            printTestDemarcation();
            goto freeMemory;
        }
        }

        if (queryStatus == QUERY_SUCCESS && expectedResult == actualResult) {
            printTestSuccess();
            result = TEST_SUCCESS;
        } else {
            printTestFailure();
            printTestDemarcation();
            if (queryStatus != QUERY_SUCCESS) {
                printTestResultDifferenceErrorCode(
                    QUERY_SUCCESS, queryingStatusToString(QUERY_SUCCESS),
                    queryStatus, queryingStatusToString(queryStatus));
            }
            printTestResultDifferenceNumber(expectedResult, actualResult);
            printTestDemarcation();
        }
    }

freeMemory:
    destroyDom(&dom);
    destroyDataContainer(&dataContainer);

    return result;
}

bool testArrayNodeQueries(size_t *successes, size_t *failures) {
    printTestTopicStart("array queries");
    size_t localSuccesses = 0;
    size_t localFailures = 0;

    for (size_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];

        printTestStart(testFile.testName);

        if (testQuery(testFile.fileLocation, testFile.cssQuery,
                      testFile.functionType,
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
}
