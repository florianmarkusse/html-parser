
#include <flo/html-parser/dom/comparison/comparison.h>
#include <flo/html-parser/dom/deletion/deletion.h>
#include <flo/html-parser/dom/modification/modification.h>
#include <flo/html-parser/dom/query/query.h>
#include <flo/html-parser/dom/user.h>
#include <flo/html-parser/utils/print/error.h>
#include <stdbool.h>
#include <stdio.h>

#include "node/modifying/modifying.h"
#include "test-status.h"
#include "test.h"

#define CURRENT_DIR "tests/src/node/modifying/inputs/"
#define TEST_FILE_1_BEFORE CURRENT_DIR "test-1-before.html"
#define TEST_FILE_1_AFTER CURRENT_DIR "test-1-after.html"
#define TEST_FILE_2_BEFORE CURRENT_DIR "test-2-before.html"
#define TEST_FILE_2_AFTER CURRENT_DIR "test-2-after.html"
#define TEST_FILE_3_BEFORE CURRENT_DIR "test-3-before.html"
#define TEST_FILE_3_AFTER CURRENT_DIR "test-3-after.html"
#define TEST_FILE_4_BEFORE CURRENT_DIR "test-4-before.html"
#define TEST_FILE_4_AFTER CURRENT_DIR "test-4-after.html"
#define TEST_FILE_5_BEFORE CURRENT_DIR "test-5-before.html"
#define TEST_FILE_5_AFTER CURRENT_DIR "test-5-after.html"
#define TEST_FILE_6_BEFORE CURRENT_DIR "test-6-before.html"
#define TEST_FILE_6_AFTER CURRENT_DIR "test-6-after.html"
#define TEST_FILE_7_BEFORE CURRENT_DIR "test-7-before.html"
#define TEST_FILE_7_AFTER CURRENT_DIR "test-7-after.html"
#define TEST_FILE_8_BEFORE CURRENT_DIR "test-8-before.html"
#define TEST_FILE_8_AFTER CURRENT_DIR "test-8-after.html"

typedef struct {
    const char *fileLocation1;
    const char *fileLocation2;
    const char *cssQuery;
    const char *propKey;
    const char *newPropValue;
    const char *testName;
} __attribute__((aligned(64))) TestFile;

static const TestFile testFiles[] = {
    {TEST_FILE_1_BEFORE, TEST_FILE_1_AFTER, "body", "style", "newstyle",
     "change property value"},
    {TEST_FILE_2_BEFORE, TEST_FILE_2_AFTER, "#text-content-test", "id",
     "id-changed", "change id value"},
};
static const size_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus testModification(const char *fileLocation1,
                                   const char *fileLocation2,
                                   const char *cssQuery, const char *propKey,
                                   const char *newPropValue) {
    Dom dom1;
    DataContainer dataContainer1;
    ElementStatus initStatus = createDataContainer(&dataContainer1);
    if (initStatus != ELEMENT_SUCCESS) {
        ERROR_WITH_CODE_ONLY(elementStatusToString(initStatus),
                             "Failed to initialize data container");
        return TEST_ERROR_INITIALIZATION;
    }
    if (createFromFile(fileLocation1, &dom1, &dataContainer1) != DOM_SUCCESS) {
        destroyDataContainer(&dataContainer1);
        return TEST_ERROR_INITIALIZATION;
    }

    Dom dom2;
    DataContainer dataContainer2;
    initStatus = createDataContainer(&dataContainer2);
    if (initStatus != ELEMENT_SUCCESS) {
        ERROR_WITH_CODE_ONLY(elementStatusToString(initStatus),
                             "Failed to initialize data container");
        return TEST_ERROR_INITIALIZATION;
    }
    if (createFromFile(fileLocation2, &dom2, &dataContainer2) != DOM_SUCCESS) {
        destroyDataContainer(&dataContainer1);
        destroyDataContainer(&dataContainer2);
        return TEST_ERROR_INITIALIZATION;
    }

    TestStatus result = TEST_FAILURE;
    node_id foundNode = 0;
    QueryStatus queryStatus =
        querySelector(cssQuery, &dom1, &dataContainer1, &foundNode);

    if (queryStatus != QUERY_SUCCESS) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceErrorCode(
            QUERY_SUCCESS, queryingStatusToString(QUERY_SUCCESS), queryStatus,
            queryingStatusToString(queryStatus));
        printTestDemarcation();
        goto free_memory;
    }

    ElementStatus elementStatus = setPropertyValue(
        foundNode, propKey, newPropValue, &dom1, &dataContainer1);
    if (elementStatus != ELEMENT_SUCCESS) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceErrorCode(
            ELEMENT_SUCCESS, elementStatusToString(ELEMENT_SUCCESS),
            elementStatus, elementStatusToString(elementStatus));
        printTestDemarcation();
        goto free_memory;
    }

    node_id nodeID1 = dom1.firstNodeID;
    node_id nodeID2 = dom2.firstNodeID;
    ComparisonStatus comp = equals(&nodeID1, &dom1, &dataContainer1, &nodeID2,
                                   &dom2, &dataContainer2);

    if (comp == COMPARISON_SUCCESS) {
        printTestSuccess();
        result = TEST_SUCCESS;
    } else {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceErrorCode(
            COMPARISON_SUCCESS, comparisonStatusToString(COMPARISON_SUCCESS),
            comp, comparisonStatusToString(comp));
        printFirstDifference(nodeID1, &dom1, &dataContainer1, nodeID2, &dom2,
                             &dataContainer2);
        printTestDemarcation();
    }

free_memory:
    destroyDataContainer(&dataContainer1);
    destroyDom(&dom1);
    destroyDataContainer(&dataContainer2);
    destroyDom(&dom1);

    return result;
}

static inline void testAndCount(const char *fileLocation1,
                                const char *fileLocation2, const char *cssQuery,
                                const char *propKey, const char *newPropValue,
                                const char *testName, size_t *localSuccesses,
                                size_t *localFailures) {
    printTestStart(testName);

    if (testModification(fileLocation1, fileLocation2, cssQuery, propKey,
                         newPropValue) == TEST_SUCCESS) {
        (*localSuccesses)++;
    } else {
        (*localFailures)++;
    }
}

bool testNodeModifications(size_t *successes, size_t *failures) {
    printTestTopicStart("node modifications");

    size_t localSuccesses = 0;
    size_t localFailures = 0;

    for (size_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];
        testAndCount(testFile.fileLocation1, testFile.fileLocation2,
                     testFile.cssQuery, testFile.propKey, testFile.newPropValue,
                     testFile.testName, &localSuccesses, &localFailures);
    }

    printTestScore(localSuccesses, localFailures);

    *successes += localSuccesses;
    *failures += localFailures;

    return localFailures > 0;
}
