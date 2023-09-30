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
} TestFile;

static const TestFile testFiles[] = {
    {TEST_FILE_1, "body", "style", "class", GET_VALUE,
     "flo_html_getValue when having key"},
    {TEST_FILE_1, "html", "langg", NULL, GET_VALUE,
     "flo_html_getValue when not having key"},
};

static const size_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus testQuery(const flo_html_String fileLocation,
                            const flo_html_String cssQuery,
                            const flo_html_String input,
                            const CharFunctionType functionType,
                            const flo_html_String expectedResult) {
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

    TestStatus result = TEST_FAILURE;
    flo_html_node_id foundNode = 0;
    flo_html_QueryStatus queryStatus =
        flo_html_querySelector(cssQuery, &dom, &textStore, &foundNode);

    if (queryStatus != QUERY_SUCCESS) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceErrorCode(
            QUERY_SUCCESS, flo_html_queryingStatusToString(QUERY_SUCCESS),
            queryStatus, flo_html_queryingStatusToString(queryStatus));
        printTestDemarcation();
    } else {
        flo_html_String actualResult = FLO_HTML_EMPTY_STRING;
        switch (functionType) {
        case GET_VALUE: {
            actualResult =
                flo_html_getValue(foundNode, input, &dom, &textStore);
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

        if ((expectedResult.len == 0 && actualResult.len == 0) ||
            (expectedResult.len > 0 &&
             flo_html_stringEquals(actualResult, expectedResult))) {
            printTestSuccess();
            result = TEST_SUCCESS;
        } else {
            printTestFailure();
            printTestDemarcation();
            printTestResultDifferenceString(expectedResult.buf,
                                            actualResult.buf);
            printTestDemarcation();
        }
    }

freeMemory:
    flo_html_destroyDom(&dom);
    flo_html_destroyTextStore(&textStore);

    return result;
}

bool testCharNodeQueries(size_t *successes, size_t *failures) {
    printTestTopicStart("char queries");
    size_t localSuccesses = 0;
    size_t localFailures = 0;

    for (size_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];

        printTestStart(testFile.testName);

        if (testQuery(
                FLO_HTML_S_LEN(testFile.fileLocation,
                               strlen(testFile.fileLocation)),
                FLO_HTML_S_LEN(testFile.cssQuery, strlen(testFile.cssQuery)),
                FLO_HTML_S_LEN(testFile.input, strlen(testFile.input)),
                testFile.functionType,
                FLO_HTML_S_LEN(testFile.expectedResult,
                               strlen(testFile.expectedResult))) !=
            TEST_SUCCESS) {
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
