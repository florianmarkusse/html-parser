#include <flo/html-parser.h>
#include <memory/arena.h>
#include <stdio.h>

#include "node/querying/querying.h"
#include "test-status.h"
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
    char *cssQuery;
    StringUnion stringUnion;
    bool expectedResult;
    BoolFunctionType boolFunctionType;
    char *testName;
} TestFile;

static TestFile testFiles[] = {
    {TEST_FILE_1,
     "section",
     {.attribute = FLO_STRING("a")},
     true,
     HAS_BOOL_PROP,
     "has bool prop"},
    {TEST_FILE_1,
     "section",
     {.attribute = FLO_STRING("x")},
     false,
     HAS_BOOL_PROP,
     "does not have bool prop"},
    {TEST_FILE_1,
     "body",
     {.attribute = FLO_STRING("style")},
     true,
     HAS_PROP_KEY,
     "has prop key"},
    {TEST_FILE_1,
     "body",
     {.attribute = FLO_STRING("lang")},
     false,
     HAS_PROP_KEY,
     "does not have prop key"},
    {TEST_FILE_1,
     "body",
     {.attribute = FLO_STRING("class")},
     true,
     HAS_PROP_VALUE,
     "has prop value"},
    {TEST_FILE_1,
     "body",
     {.attribute = FLO_STRING("big")},
     false,
     HAS_PROP_VALUE,
     "does not have prop value"},
    {TEST_FILE_1,
     "body",
     {.key = FLO_STRING("style"), .value = FLO_STRING("class")},
     true,
     HAS_PROPERTY,
     "has property"},
    {TEST_FILE_1,
     "body",
     {.key = FLO_STRING("style"), .value = FLO_STRING("clazz")},
     false,
     HAS_PROPERTY,
     "does not have property"},
};

static ptrdiff_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus testQuery(flo_String fileLocation,
                            flo_String cssQuery,
                            StringUnion stringUnion,
                            BoolFunctionType boolFunctionType,
                            bool expectedResult, flo_Arena scratch) {
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
            printTestFailure();
            printTestDemarcation();
            printf("No suitable enum was supplied!\n");
            printTestDemarcation();
            return result;
        }
        }

        if (actualResult == expectedResult) {
            printTestSuccess();
            result = TEST_SUCCESS;
        } else {
            printTestFailure();
            printTestDemarcation();
            printTestResultDifferenceBool(expectedResult, actualResult);
            printTestDemarcation();
        }
    }

    return result;
}

bool testBoolNodeQueries(ptrdiff_t *successes, ptrdiff_t *failures,
                         flo_Arena scratch) {
    printTestTopicStart("bool queries");
    ptrdiff_t localSuccesses = 0;
    ptrdiff_t localFailures = 0;

    for (ptrdiff_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];

        printTestStart(testFile.testName);

        if (testQuery(
                FLO_STRING_LEN(testFile.fileLocation,
                               strlen(testFile.fileLocation)),
                FLO_STRING_LEN(testFile.cssQuery, strlen(testFile.cssQuery)),
                testFile.stringUnion, testFile.boolFunctionType,
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
