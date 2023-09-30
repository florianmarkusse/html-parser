#include <flo/html-parser.h>
#include <flo/html-parser/utils/memory/memory.h>
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
} TestFile;

static const TestFile testFiles[] = {
    {TEST_FILE_1, "#my-first-div", 1, TEXT_CONTENT, "nested text node"},
    {TEST_FILE_1, "title", 1, TEXT_CONTENT, "simple text node"},
    {TEST_FILE_1, "span > p", 0, TEXT_CONTENT, "no text nodes"},
    {TEST_FILE_1, "#text-content-test", 5, TEXT_CONTENT,
     "multiple text nodes 1"},
    {TEST_FILE_1, "body", 7, TEXT_CONTENT, "multiple text nodes 2"},
};

static const size_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus testQuery(const flo_html_String fileLocation,
                            const flo_html_String cssQuery,
                            const ArrayFunctionType functionType,
                            const size_t expectedResult) {
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
        size_t actualResult = 0;
        switch (functionType) {
        case TEXT_CONTENT: {
            flo_html_String *results = NULL;
            queryStatus = flo_html_getTextContent(foundNode, &dom, &results,
                                                  &actualResult);
            FLO_HTML_FREE_TO_NULL(results);
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
                    QUERY_SUCCESS,
                    flo_html_queryingStatusToString(QUERY_SUCCESS), queryStatus,
                    flo_html_queryingStatusToString(queryStatus));
            }
            printTestResultDifferenceNumber(expectedResult, actualResult);
            printTestDemarcation();
        }
    }

freeMemory:
    flo_html_destroyDom(&dom);
    flo_html_destroyTextStore(&textStore);

    return result;
}

bool testArrayNodeQueries(size_t *successes, size_t *failures) {
    printTestTopicStart("array queries");
    size_t localSuccesses = 0;
    size_t localFailures = 0;

    for (size_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];

        printTestStart(testFile.testName);

        if (testQuery(
                FLO_HTML_S_LEN(testFile.fileLocation,
                               strlen(testFile.fileLocation)),
                FLO_HTML_S_LEN(testFile.cssQuery, strlen(testFile.cssQuery)),
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
