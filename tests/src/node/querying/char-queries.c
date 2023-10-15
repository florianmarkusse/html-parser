#include <flo/html-parser.h>
#include <flo/html-parser/util/memory.h>
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
    const flo_html_String cssQuery;
    const flo_html_String input;
    const flo_html_String expectedResult;
    const CharFunctionType functionType;
    const char *testName;
} TestFile;

static const TestFile testFiles[] = {
    {TEST_FILE_1, FLO_HTML_S("body"), FLO_HTML_S("style"), FLO_HTML_S("class"),
     GET_VALUE, "flo_html_getValue when having key"},
    {TEST_FILE_1, FLO_HTML_S("html"), FLO_HTML_S("langg"),
     FLO_HTML_EMPTY_STRING, GET_VALUE, "flo_html_getValue when not having key"},
};

static const ptrdiff_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus
testQuery(const flo_html_String fileLocation, const flo_html_String cssQuery,
          const flo_html_String input, const CharFunctionType functionType,
          const flo_html_String expectedResult, flo_html_Arena scratch) {
    flo_html_ParsedHTML parsed;
    if (flo_html_fromFile(fileLocation, &parsed, &scratch) != USER_SUCCESS) {
        FLO_HTML_PRINT_ERROR(
            "Failed to created DOM & TextStore from file %.*s\n",
            FLO_HTML_S_P(fileLocation));
        return TEST_ERROR_INITIALIZATION;
    }

    TestStatus result = TEST_FAILURE;
    flo_html_node_id foundNode = 0;
    flo_html_QueryStatus queryStatus =
        flo_html_querySelector(cssQuery, parsed, &foundNode, scratch);

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
            actualResult = flo_html_getValue(foundNode, input, parsed);
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
             flo_html_stringEquals(actualResult, expectedResult))) {
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
                         flo_html_Arena scratch) {
    printTestTopicStart("char queries");
    ptrdiff_t localSuccesses = 0;
    ptrdiff_t localFailures = 0;

    for (ptrdiff_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];

        printTestStart(testFile.testName);

        if (testQuery(FLO_HTML_S_LEN(testFile.fileLocation,
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
