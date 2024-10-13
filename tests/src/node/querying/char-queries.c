#include "flo/util/log.h"
#include <flo/html-parser.h>
#include <memory/arena.h>
#include <stdio.h>
#include <string.h>

#include "expectations.h"
#include "node/querying/querying.h"
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
    flo_String testName;
} TestFile;

static TestFile testFiles[] = {
    {TEST_FILE_1, FLO_STRING("body"), FLO_STRING("style"), FLO_STRING("class"),
     GET_VALUE, FLO_STRING("flo_html_getValue when having key")},
    {TEST_FILE_1, FLO_STRING("html"), FLO_STRING("langg"), FLO_EMPTY_STRING,
     GET_VALUE, FLO_STRING("flo_html_getValue when not having key")},
};

static ptrdiff_t numTestFiles = FLO_COUNTOF(testFiles);

static void testQuery(char *fileLocation, flo_String cssQuery, flo_String input,
                      CharFunctionType functionType, flo_String expectedResult,
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
        return;
    } else {
        flo_String actualResult = FLO_EMPTY_STRING;
        switch (functionType) {
        case GET_VALUE: {
            actualResult = flo_html_getValue(foundNode, input, dom);
            break;
        }
        default: {
            FLO_TEST_FAILURE {
                FLO_ERROR((FLO_STRING("No suitable enum was supplied!\n")));
            }
            return;
        }
        }

        if ((expectedResult.len == 0 && actualResult.len == 0) ||
            (expectedResult.len > 0 &&
             flo_stringEquals(actualResult, expectedResult))) {
            flo_testSuccess();
        } else {
            FLO_TEST_FAILURE {
                flo_appendExpectString(expectedResult, actualResult);
            }
        }
    }
}

void testCharNodeQueries(flo_Arena scratch) {
    FLO_TEST_TOPIC(FLO_STRING("char queries")) {
        for (ptrdiff_t i = 0; i < numTestFiles; i++) {
            TestFile testFile = testFiles[i];

            FLO_TEST(testFile.testName) {
                testQuery(testFile.fileLocation, testFile.cssQuery,
                          testFile.input, testFile.functionType,
                          testFile.expectedResult, scratch);
            }
        }
    }
}
