#include <flo/html-parser.h>
#include <stdbool.h>
#include <stdio.h>

#include "comparison-test.h"
#include "dom/comparing/comparing.h"
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
    char *fileLocation1;
    char *fileLocation2;
    flo_html_ComparisonStatus expectedStatus;
    flo_String testName;
} TestFile;

static TestFile testFiles[] = {
    {TEST_FILE_1, TEST_FILE_1, COMPARISON_SUCCESS, FLO_STRING("same file")},
    {TEST_FILE_1, TEST_FILE_2, COMPARISON_DIFFERENT_CONTENT,
     FLO_STRING("different key-value property")},
    {TEST_FILE_1, TEST_FILE_3, COMPARISON_DIFFERENT_SIZES,
     FLO_STRING("empty file")},
    {TEST_FILE_1, TEST_FILE_4, COMPARISON_DIFFERENT_SIZES,
     FLO_STRING("missing boolean property")},
    {TEST_FILE_1, TEST_FILE_5, COMPARISON_DIFFERENT_SIZES,
     FLO_STRING("missing key-value property")},
    {TEST_FILE_1, TEST_FILE_6, COMPARISON_DIFFERENT_NODE_TYPE,
     FLO_STRING("text node and document node")},
    {TEST_FILE_1, TEST_FILE_7, COMPARISON_DIFFERENT_SIZES,
     FLO_STRING("different sizes ")},
    {TEST_FILE_1, TEST_FILE_8, COMPARISON_DIFFERENT_CONTENT,
     FLO_STRING("different tags ")},
    {TEST_FILE_1, TEST_FILE_9, COMPARISON_DIFFERENT_CONTENT,
     FLO_STRING("different text nodes")},
    {TEST_FILE_1, TEST_FILE_10, COMPARISON_SUCCESS, FLO_STRING("comments")},
    {TEST_FILE_11, TEST_FILE_11_MIN, COMPARISON_SUCCESS,
     FLO_STRING("style tag")},
    {TEST_FILE_12, TEST_FILE_12_MIN, COMPARISON_SUCCESS,
     FLO_STRING("script tag")},
    {TEST_FILE_13, TEST_FILE_13_MIN, COMPARISON_SUCCESS,
     FLO_STRING("difficult style tag ")},
    {TEST_FILE_14, TEST_FILE_14_MIN, COMPARISON_SUCCESS,
     FLO_STRING("different quotes in attributes")},
    {TEST_FILE_15, TEST_FILE_15_MIN, COMPARISON_SUCCESS,
     FLO_STRING("quotes as attribute key")},
    {TEST_FILE_16, TEST_FILE_16_MIN, COMPARISON_SUCCESS,
     FLO_STRING("additional close tags")},
    {TEST_FILE_1, TEST_FILE_18, COMPARISON_SUCCESS,
     FLO_STRING("swapped boolean properties")},
    {TEST_FILE_1, TEST_FILE_19, COMPARISON_SUCCESS,
     FLO_STRING("swapped properties")},
};

static ptrdiff_t numTestFiles = FLO_COUNTOF(testFiles);

void compareFiles(char *fileLocation1, char *fileLocation2,
                  flo_html_ComparisonStatus expectedResult, flo_Arena scratch) {
    ComparisonTest comparisonTest =
        initComparisonTest(fileLocation1, fileLocation2, &scratch);

    compareWithCodeAndEndTest(&comparisonTest, expectedResult, scratch);
}

void testflo_html_DomComparisons(flo_Arena scratch) {
    FLO_TEST_TOPIC(FLO_STRING("DOM comparisons")) {
        for (ptrdiff_t i = 0; i < numTestFiles; i++) {
            TestFile testFile = testFiles[i];

            FLO_TEST(testFile.testName) {
                compareFiles(testFile.fileLocation1,

                             testFile.fileLocation2, testFile.expectedStatus,
                             scratch);
            }
        }
    }
}
