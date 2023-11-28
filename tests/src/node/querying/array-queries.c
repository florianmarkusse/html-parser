#include "log.h"
#include <flo/html-parser.h>
#include <memory/arena.h>
#include <stdio.h>
#include <string.h>

#include "expectations.h"
#include "node/querying/querying.h"
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

static ptrdiff_t numTestFiles = FLO_COUNTOF(testFiles);

static void testQuery(char *fileLocation, flo_String cssQuery,
                      ArrayFunctionType functionType, ptrdiff_t expectedResult,
                      flo_Arena scratch) {
    flo_html_Dom *dom = flo_html_createDomFromFile(fileLocation, &scratch);
    if (dom == NULL) {
        FLO_TEST_FAILURE {
            FLO_ERROR("Failed to created DOM from file ");
            FLO_ERROR(fileLocation, FLO_NEWLINE);
        }
        return;
    }

    flo_html_node_id foundNode = 0;
    flo_html_QueryStatus queryStatus =
        flo_html_querySelector(cssQuery, dom, &foundNode, scratch);

    if (queryStatus != QUERY_SUCCESS) {
        FLO_TEST_FAILURE {
            flo_appendExpectCodeWithString(
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
            FLO_TEST_FAILURE {
                FLO_ERROR((FLO_STRING("No suitable enum was supplied!\n")));
            }
            return;
        }
        }

        if (queryStatus == QUERY_SUCCESS && expectedResult == actualResult) {
            flo_testSuccess();
            return;
        } else {
            FLO_TEST_FAILURE {
                if (queryStatus != QUERY_SUCCESS) {
                    flo_appendExpectCodeWithString(
                        QUERY_SUCCESS,
                        flo_html_queryingStatusToString(QUERY_SUCCESS),
                        queryStatus,
                        flo_html_queryingStatusToString(queryStatus));
                }
                flo_appendExpectUint(expectedResult, actualResult);
            }
        }
    }
}

void testArrayNodeQueries(flo_Arena scratch) {
    FLO_TEST_TOPIC(FLO_STRING("array queries")) {
        for (ptrdiff_t i = 0; i < numTestFiles; i++) {
            TestFile testFile = testFiles[i];

            FLO_TEST(testFile.testName) {
                testQuery(testFile.fileLocation, testFile.cssQuery,
                          testFile.functionType, testFile.expectedResult,
                          scratch);
            }
        }
    }
}
