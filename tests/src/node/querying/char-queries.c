#include <flo/html-parser.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>

#include "node/querying/querying.h"
#include "test-status.h"
#include "test.h"

typedef enum { GET_VALUE, NUM_CHAR_FUNCTION_TYPES } CharFunctionType;

#define CURRENT_DIR "tests/src/node/querying/inputs/"
#define TEST_FILE_1 CURRENT_DIR "test-1.html"

typedef struct {
    char *fileLocation;
    flo_String cssQuery;
    flo_String input;
    flo_String expectedResult;
    CharFunctionType functionType;
    char *testName;
} TestFile;

static TestFile testFiles[] = {
    {TEST_FILE_1, FLO_STRING("body"), FLO_STRING("style"), FLO_STRING("class"),
     GET_VALUE, "flo_html_getValue when having key"},
    {TEST_FILE_1, FLO_STRING("html"), FLO_STRING("langg"),
     FLO_EMPTY_STRING, GET_VALUE, "flo_html_getValue when not having key"},
};

static ptrdiff_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus
testQuery(flo_String fileLocation, flo_String cssQuery,
          flo_String input, CharFunctionType functionType,
          flo_String expectedResult, flo_Arena scratch) {
    flo_html_Dom *dom = flo_html_createDomFromFile(fileLocation, &scratch);
    if (dom == NULL) {
        FLO_PRINT_ERROR("Failed to created DOM from file %.*s\n",
                             FLO_STRING_PRINT(fileLocation));
        return TEST_ERROR_INITIALIZATION;
    }

    TestStatus result = TEST_FAILURE;
    flo_html_node_id foundNode = 0;
    flo_html_QueryStatus queryStatus =
        flo_html_querySelector(cssQuery, dom, &foundNode, scratch);

    if (queryStatus != QUERY_SUCCESS) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceErrorCode(
            QUERY_SUCCESS, flo_html_queryingStatusToString(QUERY_SUCCESS),
            queryStatus, flo_html_queryingStatusToString(queryStatus));
        printTestDemarcation();
    } else {
        flo_String actualResult = FLO_EMPTY_STRING;
        switch (functionType) {
        case GET_VALUE: {
            actualResult = flo_html_getValue(foundNode, input, dom);
            break;
        }
        default: {
            printTestFailure();
            printTestDemarcation();
            printf("No suitable enum was supplied!\n");
            printTestDemarcation();
            return result;
        }
        }

        if ((expectedResult.len == 0 && actualResult.len == 0) ||
            (expectedResult.len > 0 &&
             flo_stringEquals(actualResult, expectedResult))) {
            printTestSuccess();
            result = TEST_SUCCESS;
        } else {
            printTestFailure();
            printTestDemarcation();
            printTestResultDifferenceString(expectedResult, actualResult);
            printTestDemarcation();
        }
    }

    return result;
}

bool testCharNodeQueries(ptrdiff_t *successes, ptrdiff_t *failures,
                         flo_Arena scratch) {
    printTestTopicStart("char queries");
    ptrdiff_t localSuccesses = 0;
    ptrdiff_t localFailures = 0;

    for (ptrdiff_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];

        printTestStart(testFile.testName);

        if (testQuery(FLO_STRING_LEN(testFile.fileLocation,
                                     strlen(testFile.fileLocation)),
                      testFile.cssQuery, testFile.input, testFile.functionType,
                      testFile.expectedResult, scratch) != TEST_SUCCESS) {
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
