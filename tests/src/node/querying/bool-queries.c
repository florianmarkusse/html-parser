#include "log.h"
#include <flo/html-parser.h>
#include <memory/arena.h>
#include <stdio.h>

#include "expectations.h"
#include "node/querying/querying.h"
#include "test.h"

typedef enum {
    HAS_BOOL_PROP,
    HAS_PROP_KEY,
    HAS_PROP_VALUE,
    HAS_PROPERTY,
    NUM_BOOL_FUNCTION_TYPES
} BoolFunctionType;

#define CURRENT_DIR "tests/src/node/querying/inputs/"
#define TEST_FILE_1 CURRENT_DIR "test-1.html"

typedef union {
    flo_String attribute;
    struct {
        flo_String key;
        flo_String value;
    } __attribute__((aligned(16)));
} StringUnion;

typedef struct {
    char *fileLocation;
    flo_String cssQuery;
    StringUnion stringUnion;
    bool expectedResult;
    BoolFunctionType boolFunctionType;
    flo_String testName;
} TestFile;

static TestFile testFiles[] = {
    {TEST_FILE_1,
     FLO_STRING("section"),
     {.attribute = FLO_STRING("a")},
     true,
     HAS_BOOL_PROP,
     FLO_STRING("has bool prop")},
    {TEST_FILE_1,
     FLO_STRING("section"),
     {.attribute = FLO_STRING("x")},
     false,
     HAS_BOOL_PROP,
     FLO_STRING("does not have bool prop")},
    {TEST_FILE_1,
     FLO_STRING("body"),
     {.attribute = FLO_STRING("style")},
     true,
     HAS_PROP_KEY,
     FLO_STRING("has prop key")},
    {TEST_FILE_1,
     FLO_STRING("body"),
     {.attribute = FLO_STRING("lang")},
     false,
     HAS_PROP_KEY,
     FLO_STRING("does not have prop key")},
    {TEST_FILE_1,
     FLO_STRING("body"),
     {.attribute = FLO_STRING("class")},
     true,
     HAS_PROP_VALUE,
     FLO_STRING("has prop value")},
    {TEST_FILE_1,
     FLO_STRING("body"),
     {.attribute = FLO_STRING("big")},
     false,
     HAS_PROP_VALUE,
     FLO_STRING("does not have prop value")},
    {TEST_FILE_1,
     FLO_STRING("body"),
     {.key = FLO_STRING("style"), .value = FLO_STRING("class")},
     true,
     HAS_PROPERTY,
     FLO_STRING("has property")},
    {TEST_FILE_1,
     FLO_STRING("body"),
     {.key = FLO_STRING("style"), .value = FLO_STRING("clazz")},
     false,
     HAS_PROPERTY,
     FLO_STRING("does not have property")},
};

static ptrdiff_t numTestFiles = FLO_COUNTOF(testFiles);

static void testQuery(char *fileLocation, flo_String cssQuery,
                      StringUnion stringUnion,
                      BoolFunctionType boolFunctionType, bool expectedResult,
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
        bool actualResult = false;
        switch (boolFunctionType) {
        case HAS_BOOL_PROP: {
            actualResult =
                flo_html_hasBoolProp(foundNode, stringUnion.attribute, dom);
            break;
        }
        case HAS_PROP_KEY: {
            actualResult =
                flo_html_hasPropKey(foundNode, stringUnion.attribute, dom);
            break;
        }
        case HAS_PROP_VALUE: {
            actualResult =
                flo_html_hasPropValue(foundNode, stringUnion.attribute, dom);
            break;
        }
        case HAS_PROPERTY: {
            actualResult = flo_html_hasProperty(foundNode, stringUnion.key,
                                                stringUnion.value, dom);
            break;
        }
        default: {
            FLO_TEST_FAILURE {
                FLO_ERROR((FLO_STRING("No suitable enum was supplied!\n")));
            }
            return;
        }
        }

        if (actualResult == expectedResult) {
            flo_testSuccess();
            return;
        } else {
            FLO_TEST_FAILURE {
                flo_appendExpectBool(expectedResult, actualResult);
            }
        }
    }
}

void testBoolNodeQueries(flo_Arena scratch) {
    FLO_TEST_TOPIC(FLO_STRING("bool queries")) {
        for (ptrdiff_t i = 0; i < numTestFiles; i++) {
            TestFile testFile = testFiles[i];

            FLO_TEST(testFile.testName) {
                testQuery(testFile.fileLocation, testFile.cssQuery,
                          testFile.stringUnion, testFile.boolFunctionType,
                          testFile.expectedResult, scratch);
            }
        }
    }
}
