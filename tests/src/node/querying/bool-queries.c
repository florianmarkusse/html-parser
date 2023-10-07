#include <flo/html-parser.h>
#include <flo/html-parser/util/memory.h>
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
    const flo_html_String attribute;
    struct {
        const flo_html_String key;
        const flo_html_String value;
    } __attribute__((aligned(16)));
} StringUnion;

typedef struct {
    const char *fileLocation;
    const char *cssQuery;
    StringUnion stringUnion;
    const bool expectedResult;
    const BoolFunctionType boolFunctionType;
    const char *testName;
} TestFile;

static const TestFile testFiles[] = {
    {TEST_FILE_1,
     "section",
     {.attribute = FLO_HTML_S("a")},
     true,
     HAS_BOOL_PROP,
     "has bool prop"},
    {TEST_FILE_1,
     "section",
     {.attribute = FLO_HTML_S("x")},
     false,
     HAS_BOOL_PROP,
     "does not have bool prop"},
    {TEST_FILE_1,
     "body",
     {.attribute = FLO_HTML_S("style")},
     true,
     HAS_PROP_KEY,
     "has prop key"},
    {TEST_FILE_1,
     "body",
     {.attribute = FLO_HTML_S("lang")},
     false,
     HAS_PROP_KEY,
     "does not have prop key"},
    {TEST_FILE_1,
     "body",
     {.attribute = FLO_HTML_S("class")},
     true,
     HAS_PROP_VALUE,
     "has prop value"},
    {TEST_FILE_1,
     "body",
     {.attribute = FLO_HTML_S("big")},
     false,
     HAS_PROP_VALUE,
     "does not have prop value"},
    {TEST_FILE_1,
     "body",
     {.key = FLO_HTML_S("style"), .value = FLO_HTML_S("class")},
     true,
     HAS_PROPERTY,
     "has property"},
    {TEST_FILE_1,
     "body",
     {.key = FLO_HTML_S("style"), .value = FLO_HTML_S("clazz")},
     false,
     HAS_PROPERTY,
     "does not have property"},
};

static const ptrdiff_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus testQuery(const flo_html_String fileLocation,
                            const flo_html_String cssQuery,
                            const StringUnion stringUnion,
                            const BoolFunctionType boolFunctionType,
                            const bool expectedResult) {
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
        bool actualResult = false;
        switch (boolFunctionType) {
        case HAS_BOOL_PROP: {
            actualResult = flo_html_hasBoolProp(
                foundNode, stringUnion.attribute, &dom, &textStore);
            break;
        }
        case HAS_PROP_KEY: {
            actualResult = flo_html_hasPropKey(foundNode, stringUnion.attribute,
                                               &dom, &textStore);
            break;
        }
        case HAS_PROP_VALUE: {
            actualResult = flo_html_hasPropValue(
                foundNode, stringUnion.attribute, &dom, &textStore);
            break;
        }
        case HAS_PROPERTY: {
            actualResult =
                flo_html_hasProperty(foundNode, stringUnion.key,
                                     stringUnion.value, &dom, &textStore);
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

freeMemory:
    flo_html_destroyDom(&dom);
    flo_html_destroyTextStore(&textStore);

    return result;
}

bool testBoolNodeQueries(ptrdiff_t *successes, ptrdiff_t *failures) {
    printTestTopicStart("bool queries");
    ptrdiff_t localSuccesses = 0;
    ptrdiff_t localFailures = 0;

    for (ptrdiff_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];

        printTestStart(testFile.testName);

        if (testQuery(
                FLO_HTML_S_LEN(testFile.fileLocation,
                               strlen(testFile.fileLocation)),
                FLO_HTML_S_LEN(testFile.cssQuery, strlen(testFile.cssQuery)),
                testFile.stringUnion, testFile.boolFunctionType,
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
