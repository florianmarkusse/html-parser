#include <flo/html-parser.h>
#include <flo/html-parser/utils/memory/memory.h>
#include <stdio.h>
#include <string.h>

#include "node/querying/querying.h"
#include "test-status.h"
#include "test.h"

typedef enum { GET_VALUE, NUM_CHAR_FUNCTION_TYPES } CharFunctionType;

#define CURRENT_DIR "tests/src/node/querying/inputs/"
#define TEST_FILE_1 CURRENT_DIR "test-1.html"

typedef struct {
    const char *fileLocation;
    const char *cssQuery;
    const char *input;
    const char *expectedResult;
    const CharFunctionType functionType;
    const char *testName;
} __attribute__((aligned(64))) TestFile;

static const TestFile testFiles[] = {
    {TEST_FILE_1, "body", "style", "class", GET_VALUE,
     "getValue when having key"},
    {TEST_FILE_1, "html", "langg", NULL, GET_VALUE,
     "getValue when not having key"},
};

static const size_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus testQuery(const char *fileLocation, const char *cssQuery,
                            const char *input,
                            const CharFunctionType functionType,
                            const char *expectedResult) {
    TextStore textStore;
    ElementStatus initStatus = createTextStore(&textStore);
    if (initStatus != ELEMENT_SUCCESS) {
        ERROR_WITH_CODE_ONLY(elementStatusToString(initStatus),
                             "Failed to initialize text store");
        return TEST_ERROR_INITIALIZATION;
    }

    Dom dom;
    if (createDomFromFile(fileLocation, &dom, &textStore) != DOM_SUCCESS) {
        destroyTextStore(&textStore);
        return TEST_ERROR_INITIALIZATION;
    }

    TestStatus result = TEST_FAILURE;
    node_id foundNode = 0;
    QueryStatus queryStatus =
        querySelector(cssQuery, &dom, &textStore, &foundNode);

    if (queryStatus != QUERY_SUCCESS) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceErrorCode(
            QUERY_SUCCESS, queryingStatusToString(QUERY_SUCCESS), queryStatus,
            queryingStatusToString(queryStatus));
        printTestDemarcation();
    } else {
        const char *actualResult = NULL;
        switch (functionType) {
        case GET_VALUE: {
            actualResult = getValue(foundNode, input, &dom, &textStore);
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

        if ((expectedResult == NULL && actualResult == NULL) ||
            (expectedResult != NULL &&
             strcmp(actualResult, expectedResult) == 0)) {
            printTestSuccess();
            result = TEST_SUCCESS;
        } else {
            printTestFailure();
            printTestDemarcation();
            printTestResultDifferenceString(expectedResult, actualResult);
            printTestDemarcation();
        }
    }

freeMemory:
    destroyDom(&dom);
    destroyTextStore(&textStore);

    return result;
}

bool testCharNodeQueries(size_t *successes, size_t *failures) {
    printTestTopicStart("char queries");
    size_t localSuccesses = 0;
    size_t localFailures = 0;

    for (size_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];

        printTestStart(testFile.testName);

        if (testQuery(testFile.fileLocation, testFile.cssQuery, testFile.input,
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
