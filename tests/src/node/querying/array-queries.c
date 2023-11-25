#include "log.h"
#include <flo/html-parser.h>
#include <memory/arena.h>
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
    flo_String cssQuery;
    ptrdiff_t expectedResult;
    ArrayFunctionType functionType;
    flo_String testName;
} TestFile;

static TestFile testFiles[] = {
    {TEST_FILE_1, FLO_STRING("#my-first-div"), 1, TEXT_CONTENT,
     FLO_STRING("nested text node")},
    {TEST_FILE_1, FLO_STRING("title"), 1, TEXT_CONTENT,
     FLO_STRING("simple text node")},
    {TEST_FILE_1, FLO_STRING("span > p"), 0, TEXT_CONTENT,
     FLO_STRING("no text nodes")},
    {TEST_FILE_1, FLO_STRING("#text-content-test"), 5, TEXT_CONTENT,
     FLO_STRING("multiple text nodes 1")},
    {TEST_FILE_1, FLO_STRING("body"), 7, TEXT_CONTENT,
     FLO_STRING("multiple text nodes 2")},
};

static ptrdiff_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus testQuery(flo_String fileLocation, flo_String cssQuery,
                            ArrayFunctionType functionType,
                            ptrdiff_t expectedResult, flo_Arena scratch) {
    flo_html_Dom *dom = flo_html_createDomFromFile(fileLocation, &scratch);
    if (dom == NULL) {
        FLO_LOG_TEST_FAILED {
            FLO_ERROR("Failed to created DOM from file ");
            FLO_ERROR(fileLocation, FLO_NEWLINE);
        }
        return TEST_ERROR_INITIALIZATION;
    }

    TestStatus result = TEST_FAILURE;
    flo_html_node_id foundNode = 0;
    flo_html_QueryStatus queryStatus =
        flo_html_querySelector(cssQuery, dom, &foundNode, scratch);

    if (queryStatus != QUERY_SUCCESS) {
        FLO_LOG_TEST_FAILED {
            printTestResultDifferenceErrorCode(
                QUERY_SUCCESS, flo_html_queryingStatusToString(QUERY_SUCCESS),
                queryStatus, flo_html_queryingStatusToString(queryStatus));
        }
    } else {
        ptrdiff_t actualResult = 0;
        switch (functionType) {
        case TEXT_CONTENT: {
            flo_String_d_a results =
                flo_html_getTextContent(foundNode, dom, &scratch);
            actualResult = results.len;
            break;
        }
        default: {
            FLO_LOG_TEST_FAILED {
                FLO_ERROR((FLO_STRING("No suitable enum was supplied!\n")));
            }
            return result;
        }
        }

        if (queryStatus == QUERY_SUCCESS && expectedResult == actualResult) {
            printTestSuccess();
            result = TEST_SUCCESS;
        } else {
            FLO_LOG_TEST_FAILED {
                if (queryStatus != QUERY_SUCCESS) {
                    printTestResultDifferenceErrorCode(
                        QUERY_SUCCESS,
                        flo_html_queryingStatusToString(QUERY_SUCCESS),
                        queryStatus,
                        flo_html_queryingStatusToString(queryStatus));
                }
                printTestResultDifferenceNumber(expectedResult, actualResult);
            }
        }
    }

    return result;
}

bool testArrayNodeQueries(ptrdiff_t *successes, ptrdiff_t *failures,
                          flo_Arena scratch) {
    printTestTopicStart(FLO_STRING("array queries"));
    ptrdiff_t localSuccesses = 0;
    ptrdiff_t localFailures = 0;

    for (ptrdiff_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];

        printTestStart(testFile.testName);

        if (testQuery(FLO_STRING_LEN(testFile.fileLocation,
                                     strlen(testFile.fileLocation)),
                      testFile.cssQuery, testFile.functionType,
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
