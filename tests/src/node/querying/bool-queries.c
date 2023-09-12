#include <flo/html-parser.h>
#include <flo/html-parser/utils/memory/memory.h>
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
    const char *attribute;
    struct {
        const char *key;
        const char *value;
    } __attribute__((aligned(16)));
} StringUnion;

typedef struct {
    const char *fileLocation;
    const char *cssQuery;
    StringUnion stringUnion;
    const bool expectedResult;
    const BoolFunctionType boolFunctionType;
    const char *testName;
} __attribute__((aligned(64))) TestFile;

static const TestFile testFiles[] = {
    {TEST_FILE_1,
     "section",
     {.attribute = "a"},
     true,
     HAS_BOOL_PROP,
     "has bool prop"},
    {TEST_FILE_1,
     "section",
     {.attribute = "x"},
     false,
     HAS_BOOL_PROP,
     "does not have bool prop"},
    {TEST_FILE_1,
     "body",
     {.attribute = "style"},
     true,
     HAS_PROP_KEY,
     "has prop key"},
    {TEST_FILE_1,
     "body",
     {.attribute = "lang"},
     false,
     HAS_PROP_KEY,
     "does not have prop key"},
    {TEST_FILE_1,
     "body",
     {.attribute = "class"},
     true,
     HAS_PROP_VALUE,
     "has prop value"},
    {TEST_FILE_1,
     "body",
     {.attribute = "big"},
     false,
     HAS_PROP_VALUE,
     "does not have prop value"},
    {TEST_FILE_1,
     "body",
     {.key = "style", .value = "class"},
     true,
     HAS_PROPERTY,
     "has property"},
    {TEST_FILE_1,
     "body",
     {.key = "style", .value = "clazz"},
     false,
     HAS_PROPERTY,
     "does not have property"},
};

static const size_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus testQuery(const char *fileLocation, const char *cssQuery,
                            const StringUnion stringUnion,
                            const BoolFunctionType boolFunctionType,
                            const bool expectedResult) {
    TextStore textStore;
    ElementStatus initStatus = createTextStore(&textStore);
    if (initStatus != ELEMENT_SUCCESS) {
        ERROR_WITH_CODE_ONLY(elementStatusToString(initStatus),
                             "Failed to initialize text store");
        return TEST_ERROR_INITIALIZATION;
    }

    Dom dom;
    if (createDomFromFile(fileLocation, &dom, &textStore) != DOM_SUCCESS) {
        destroyTextStore(&textStore);
        return TEST_ERROR_INITIALIZATION;
    }

    TestStatus result = TEST_FAILURE;
    node_id foundNode = 0;
    QueryStatus queryStatus =
        querySelector(cssQuery, &dom, &textStore, &foundNode);

    if (queryStatus != QUERY_SUCCESS) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceErrorCode(
            QUERY_SUCCESS, queryingStatusToString(QUERY_SUCCESS), queryStatus,
            queryingStatusToString(queryStatus));
        printTestDemarcation();
    } else {
        bool actualResult = false;
        switch (boolFunctionType) {
        case HAS_BOOL_PROP: {
            actualResult = hasBoolProp(foundNode, stringUnion.attribute, &dom,
                                       &textStore);
            break;
        }
        case HAS_PROP_KEY: {
            actualResult = hasPropKey(foundNode, stringUnion.attribute, &dom,
                                      &textStore);
            break;
        }
        case HAS_PROP_VALUE: {
            actualResult = hasPropValue(foundNode, stringUnion.attribute, &dom,
                                        &textStore);
            break;
        }
        case HAS_PROPERTY: {
            actualResult = hasProperty(foundNode, stringUnion.key,
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
    destroyDom(&dom);
    destroyTextStore(&textStore);

    return result;
}

bool testBoolNodeQueries(size_t *successes, size_t *failures) {
    printTestTopicStart("bool queries");
    size_t localSuccesses = 0;
    size_t localFailures = 0;

    for (size_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];

        printTestStart(testFile.testName);

        if (testQuery(testFile.fileLocation, testFile.cssQuery,
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
