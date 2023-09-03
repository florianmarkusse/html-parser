#include <flo/html-parser/dom/comparison/comparison.h>
#include <flo/html-parser/dom/user.h>
#include <flo/html-parser/utils/print/error.h>
#include <stdbool.h>
#include <stdio.h>

#include "comparison-test.h"
#include "dom/comparing/comparing.h"
#include "test-status.h"
#include "test.h"

#define CURRENT_DIR "tests/src/dom/comparing/inputs/"
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
#define TEST_FILE_18 CURRENT_DIR "test-18.html"
#define TEST_FILE_19 CURRENT_DIR "test-19.html"

typedef struct {
    const char *fileLocation1;
    const char *fileLocation2;
    ComparisonStatus expectedStatus;
    const char *testName;
} __attribute__((aligned(32))) TestFile;

static const TestFile testFiles[] = {
    {TEST_FILE_1, TEST_FILE_1, COMPARISON_SUCCESS, "same file"},
    {TEST_FILE_1, TEST_FILE_2, COMPARISON_DIFFERENT_CONTENT,
     "different key-value property"},
    {TEST_FILE_1, TEST_FILE_3, COMPARISON_DIFFERENT_SIZES, "empty file"},
    {TEST_FILE_1, TEST_FILE_4, COMPARISON_DIFFERENT_SIZES,
     "missing boolean property"},
    {TEST_FILE_1, TEST_FILE_5, COMPARISON_DIFFERENT_SIZES,
     "missing key-value property"},
    {TEST_FILE_1, TEST_FILE_6, COMPARISON_DIFFERENT_NODE_TYPE,
     "text node and document node"},
    {TEST_FILE_1, TEST_FILE_7, COMPARISON_DIFFERENT_SIZES, "different sizes"},
    {TEST_FILE_1, TEST_FILE_8, COMPARISON_DIFFERENT_CONTENT, "different tags"},
    {TEST_FILE_1, TEST_FILE_9, COMPARISON_DIFFERENT_CONTENT,
     "different text nodes"},
    {TEST_FILE_1, TEST_FILE_10, COMPARISON_SUCCESS, "comments"},
    {TEST_FILE_11, TEST_FILE_11_MIN, COMPARISON_SUCCESS, "style tag"},
    {TEST_FILE_12, TEST_FILE_12_MIN, COMPARISON_SUCCESS, "script tag"},
    {TEST_FILE_13, TEST_FILE_13_MIN, COMPARISON_SUCCESS, "difficult style tag"},
    {TEST_FILE_14, TEST_FILE_14_MIN, COMPARISON_SUCCESS,
     "different quotes in attributes"},
    {TEST_FILE_15, TEST_FILE_15_MIN, COMPARISON_SUCCESS,
     "quotes as attribute key"},
    {TEST_FILE_16, TEST_FILE_16_MIN, COMPARISON_SUCCESS,
     "additional close tags"},
    {TEST_FILE_17, TEST_FILE_17_MIN, COMPARISON_SUCCESS,
     "key-value property without quotes"},
    {TEST_FILE_1, TEST_FILE_18, COMPARISON_SUCCESS,
     "swapped boolean properties"},
    {TEST_FILE_1, TEST_FILE_19, COMPARISON_SUCCESS, "swapped properties"},
};

static const size_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

TestStatus compareFiles(const char *fileLocation1, const char *fileLocation2,
                        const ComparisonStatus expectedResult) {
    TestStatus result = TEST_FAILURE;

    ComparisonTest comparisonTest;
    result = initComparisonTest(&comparisonTest, fileLocation1, fileLocation2);
    if (result != TEST_SUCCESS) {
        return result;
    }

    return compareWithCodeAndEndTest(&comparisonTest, expectedResult);
}

bool testDomComparisons(size_t *successes, size_t *failures) {
    printTestTopicStart("DOM comparisons");
    size_t localSuccesses = 0;
    size_t localFailures = 0;

    for (size_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];

        printTestStart(testFile.testName);

        if (compareFiles(testFile.fileLocation1, testFile.fileLocation2,
                         testFile.expectedStatus) != TEST_SUCCESS) {
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
