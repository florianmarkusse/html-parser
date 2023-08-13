#include <stdio.h>

#include "comparisons/comparisons.h"
#include "dom/comparison/document-comparison.h"
#include "dom/document-user.h"
#include "dom/document-writing.h"
#include "dom/document.h"
#include "test-status.h"
#include "test.h"

#define CURRENT_DIR "tests/src/comparisons/inputs/"
#define TEST_FILE_1 CURRENT_DIR "test-1.html"
#define TEST_FILE_2 CURRENT_DIR "test-2.html"
#define TEST_FILE_3 CURRENT_DIR "test-3.html"
#define TEST_FILE_4 CURRENT_DIR "test-4.html"
#define TEST_FILE_5 CURRENT_DIR "test-5.html"
#define TEST_FILE_6 CURRENT_DIR "test-6.html"
#define TEST_FILE_7 CURRENT_DIR "test-7.html"
#define TEST_FILE_8 CURRENT_DIR "test-8.html"
#define TEST_FILE_9 CURRENT_DIR "test-9.html"
#define TEST_FILE_10 CURRENT_DIR "test-10.html"
#define TEST_FILE_11 CURRENT_DIR "test-11.html"
#define TEST_FILE_11_MIN CURRENT_DIR "test-11-min.html"
#define TEST_FILE_12 CURRENT_DIR "test-12.html"
#define TEST_FILE_12_MIN CURRENT_DIR "test-12-min.html"
#define TEST_FILE_13 CURRENT_DIR "test-13.html"
#define TEST_FILE_13_MIN CURRENT_DIR "test-13-min.html"
#define TEST_FILE_14 CURRENT_DIR "test-14.html"
#define TEST_FILE_14_MIN CURRENT_DIR "test-14-min.html"
#define TEST_FILE_15 CURRENT_DIR "test-15.html"
#define TEST_FILE_15_MIN CURRENT_DIR "test-15-min.html"
#define TEST_FILE_16 CURRENT_DIR "test-16.html"
#define TEST_FILE_16_MIN CURRENT_DIR "test-16-min.html"
#define TEST_FILE_17 CURRENT_DIR "test-17.html"
#define TEST_FILE_17_MIN CURRENT_DIR "test-17-min.html"

TestStatus compareFiles(const char *fileLocation1,
                        DataContainer *dataContainer1,
                        const char *fileLocation2,
                        DataContainer *dataContainer2,
                        const ComparisonStatus expectedResult) {
    Document doc1;
    if (createFromFile(fileLocation1, &doc1, dataContainer1) !=
        DOCUMENT_SUCCESS) {
        destroyDataContainer(dataContainer1);
        destroyDataContainer(dataContainer2);
        return TEST_ERROR_INITIALIZATION;
    }

    Document doc2;
    if (createFromFile(fileLocation2, &doc2, dataContainer2) !=
        DOCUMENT_SUCCESS) {
        destroyDataContainer(dataContainer1);
        destroyDataContainer(dataContainer2);
        return TEST_ERROR_INITIALIZATION;
    }

    node_id nodeID1 = 0;
    node_id nodeID2 = 0;
    ComparisonStatus comp = equals(&nodeID1, &doc1, dataContainer1, &nodeID2,
                                   &doc2, dataContainer2);

    TestStatus result = TEST_FAILURE;

    if (comp == expectedResult) {
        printTestSuccess();
        result = TEST_SUCCESS;
    } else {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceString(
            expectedResult, comparisonStatusToString(expectedResult), comp,
            comparisonStatusToString(comp));
        printFirstDifference(nodeID1, &doc1, dataContainer1, nodeID2, &doc2,
                             dataContainer2);
        printTestDemarcation();
    }

    destroyDocument(&doc1);
    destroyDocument(&doc2);

    destroyDataContainer(dataContainer1);
    destroyDataContainer(dataContainer2);

    return result;
}

TestStatus
compareFilesDiffDataContainer(const char *fileLocation1,
                              const char *fileLocation2,
                              const ComparisonStatus expectedResult) {
    DataContainer dataContainer1;
    createDataContainer(&dataContainer1);
    DataContainer dataContainer2;
    createDataContainer(&dataContainer2);

    return compareFiles(fileLocation1, &dataContainer1, fileLocation2,
                        &dataContainer2, expectedResult);
}

TestStatus
compareFilesSameDataContainer(const char *fileLocation1,
                              const char *fileLocation2,
                              const ComparisonStatus expectedResult) {
    DataContainer dataContainer;
    createDataContainer(&dataContainer);

    return compareFiles(fileLocation1, &dataContainer, fileLocation2,
                        &dataContainer, expectedResult);
}

static inline void sameContainerParseAndCompare(
    const char *fileLocation1, const char *fileLocation2,
    const ComparisonStatus expectedResult, const char *testName,
    size_t *localSuccesses, size_t *localFailures) {
    printTestStart(testName);

    if (compareFilesSameDataContainer(fileLocation1, fileLocation2,
                                      expectedResult) == TEST_SUCCESS) {
        (*localSuccesses)++;
    } else {
        (*localFailures)++;
    }
}

static inline void diffContainerParseAndCompare(
    const char *fileLocation1, const char *fileLocation2,
    const ComparisonStatus expectedResult, const char *testName,
    size_t *localSuccesses, size_t *localFailures) {
    printTestStart(testName);

    if (compareFilesDiffDataContainer(fileLocation1, fileLocation2,
                                      expectedResult) == TEST_SUCCESS) {
        (*localSuccesses)++;
    } else {
        (*localFailures)++;
    }
}

unsigned char testComparisons(size_t *successes, size_t *failures) {
    printTestTopicStart("document comparisons");
    size_t localSuccesses = 0;
    size_t localFailures = 0;

    sameContainerParseAndCompare(TEST_FILE_1, TEST_FILE_1, COMPARISON_SUCCESS,
                                 "same file, same container", &localSuccesses,
                                 &localFailures);
    diffContainerParseAndCompare(TEST_FILE_1, TEST_FILE_1, COMPARISON_SUCCESS,
                                 "same file, different container",
                                 &localSuccesses, &localFailures);
    sameContainerParseAndCompare(
        TEST_FILE_1, TEST_FILE_2, COMPARISON_DIFFERENT_PROPERTIES,
        "different key-value property", &localSuccesses, &localFailures);
    sameContainerParseAndCompare(TEST_FILE_1, TEST_FILE_3,
                                 COMPARISON_DIFFERENT_SIZES, "empty file",
                                 &localSuccesses, &localFailures);
    sameContainerParseAndCompare(
        TEST_FILE_1, TEST_FILE_4, COMPARISON_MISSING_PROPERTIES,
        "missing boolean property", &localSuccesses, &localFailures);
    sameContainerParseAndCompare(
        TEST_FILE_1, TEST_FILE_5, COMPARISON_MISSING_PROPERTIES,
        "missing key-value property", &localSuccesses, &localFailures);
    sameContainerParseAndCompare(
        TEST_FILE_1, TEST_FILE_6, COMPARISON_DIFFERENT_NODE_TYPE,
        "text node and document node", &localSuccesses, &localFailures);
    sameContainerParseAndCompare(TEST_FILE_1, TEST_FILE_7,
                                 COMPARISON_DIFFERENT_SIZES, "different sizes",
                                 &localSuccesses, &localFailures);
    sameContainerParseAndCompare(TEST_FILE_1, TEST_FILE_8,
                                 COMPARISON_DIFFERENT_TAGS, "different tags",
                                 &localSuccesses, &localFailures);
    sameContainerParseAndCompare(
        TEST_FILE_1, TEST_FILE_9, COMPARISON_DIFFERENT_TEXT,
        "different text nodes", &localSuccesses, &localFailures);
    sameContainerParseAndCompare(TEST_FILE_1, TEST_FILE_10, COMPARISON_SUCCESS,
                                 "comments", &localSuccesses, &localFailures);
    sameContainerParseAndCompare(TEST_FILE_11, TEST_FILE_11_MIN,
                                 COMPARISON_SUCCESS, "style tag",
                                 &localSuccesses, &localFailures);
    sameContainerParseAndCompare(TEST_FILE_12, TEST_FILE_12_MIN,
                                 COMPARISON_SUCCESS, "script tag",
                                 &localSuccesses, &localFailures);
    sameContainerParseAndCompare(TEST_FILE_13, TEST_FILE_13_MIN,
                                 COMPARISON_SUCCESS, "difficult style tag",
                                 &localSuccesses, &localFailures);
    sameContainerParseAndCompare(
        TEST_FILE_14, TEST_FILE_14_MIN, COMPARISON_SUCCESS,
        "different quotes in attributes", &localSuccesses, &localFailures);
    sameContainerParseAndCompare(TEST_FILE_15, TEST_FILE_15_MIN,
                                 COMPARISON_SUCCESS, "quotes as attribute key",
                                 &localSuccesses, &localFailures);
    sameContainerParseAndCompare(TEST_FILE_16, TEST_FILE_16_MIN,
                                 COMPARISON_SUCCESS, "additional close tags",
                                 &localSuccesses, &localFailures);
    sameContainerParseAndCompare(
        TEST_FILE_17, TEST_FILE_17_MIN, COMPARISON_SUCCESS,
        "key-value property without quotes", &localSuccesses, &localFailures);

    printTestScore(localSuccesses, localFailures);

    *successes += localSuccesses;
    *failures += localFailures;

    return localFailures > 0;
}
