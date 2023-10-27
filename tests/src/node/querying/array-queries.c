#include <flo/html-parser.h>
#include <flo/html-parser/util/memory.h>
#include <stdio.h>
#include <string.h>

#include "node/querying/querying.h"
#include "test-status.h"
#include "test.h"

typedef enum { TEXT_CONTENT, NUM_CHAR_FUNCTION_TYPES } ArrayFunctionType;

#define CURRENT_DIR "tests/src/node/querying/inputs/"
#define TEST_FILE_1 CURRENT_DIR "test-1.html"

typedef struct {
    char *fileLocation;
    char *cssQuery;
    ptrdiff_t expectedResult;
    ArrayFunctionType functionType;
    char *testName;
} TestFile;

static TestFile testFiles[] = {
    {TEST_FILE_1, "#my-first-div", 1, TEXT_CONTENT, "nested text node"},
    {TEST_FILE_1, "title", 1, TEXT_CONTENT, "simple text node"},
    {TEST_FILE_1, "span > p", 0, TEXT_CONTENT, "no text nodes"},
    {TEST_FILE_1, "#text-content-test", 5, TEXT_CONTENT,
     "multiple text nodes 1"},
    {TEST_FILE_1, "body", 7, TEXT_CONTENT, "multiple text nodes 2"},
};

static ptrdiff_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus testQuery(flo_html_String fileLocation,
                            flo_html_String cssQuery,
                            ArrayFunctionType functionType,
                            ptrdiff_t expectedResult, flo_html_Arena scratch) {
    flo_html_Dom *dom = flo_html_createDomFromFile(fileLocation, &scratch);
    if (dom == NULL) {
        FLO_HTML_PRINT_ERROR("Failed to created DOM from file %.*s\n",
                             FLO_HTML_S_P(fileLocation));
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
        ptrdiff_t actualResult = 0;
        switch (functionType) {
        case TEXT_CONTENT: {
            flo_html_String_d_a results =
                flo_html_getTextContent(foundNode, dom, &scratch);
            actualResult = results.len;
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

        if (queryStatus == QUERY_SUCCESS && expectedResult == actualResult) {
            printTestSuccess();
            result = TEST_SUCCESS;
        } else {
            printTestFailure();
            printTestDemarcation();
            if (queryStatus != QUERY_SUCCESS) {
                printTestResultDifferenceErrorCode(
                    QUERY_SUCCESS,
                    flo_html_queryingStatusToString(QUERY_SUCCESS), queryStatus,
                    flo_html_queryingStatusToString(queryStatus));
            }
            printTestResultDifferenceNumber(expectedResult, actualResult);
            printTestDemarcation();
        }
    }

    return result;
}

bool testArrayNodeQueries(ptrdiff_t *successes, ptrdiff_t *failures,
                          flo_html_Arena scratch) {
    printTestTopicStart("array queries");
    ptrdiff_t localSuccesses = 0;
    ptrdiff_t localFailures = 0;

    for (ptrdiff_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];

        printTestStart(testFile.testName);

        if (testQuery(
                FLO_HTML_S_LEN(testFile.fileLocation,
                               strlen(testFile.fileLocation)),
                FLO_HTML_S_LEN(testFile.cssQuery, strlen(testFile.cssQuery)),
                testFile.functionType, testFile.expectedResult,
                scratch) != TEST_SUCCESS) {
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
