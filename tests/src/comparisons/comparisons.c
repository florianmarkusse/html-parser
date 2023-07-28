#include <stdio.h>

#include "comparisons/comparisons.h"
#include "dom/document-comparison.h"
#include "dom/document-user.h"
#include "dom/document.h"
#include "pretty-print.h"
#include "test-status.h"
#include "utils/print/error.h"

#define CURRENT_DIR "tests/src/comparisons/"
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

TestStatus compareFiles(const char *fileLocation1, const char *fileLocation2,
                        const ComparisonStatus expectedResult,
                        const char *testName) {
    printf("    Testing %-50s", testName);

    createGlobals();

    Document doc1;
    if (createFromFile(fileLocation1, &doc1) != DOCUMENT_SUCCESS) {
        destroyGlobals();
        return TEST_ERROR_INITIALIZATION;
    }

    Document doc2;
    if (createFromFile(fileLocation2, &doc2) != DOCUMENT_SUCCESS) {
        destroyGlobals();
        return TEST_ERROR_INITIALIZATION;
    }

    node_id nodeID1 = 0;
    node_id nodeID2 = 0;
    ComparisonStatus comp = equals(&doc1, &nodeID1, &doc2, &nodeID2);

    TestStatus result = TEST_FAILURE;

    if (comp == expectedResult) {
        printf("%s%-20s%s\n", ANSI_COLOR_GREEN, "Success", ANSI_COLOR_RESET);
        result = TEST_SUCCESS;
    } else {
        printf("%s%-20s%s\n", ANSI_COLOR_RED, "Failure", ANSI_COLOR_RESET);
        printf("---------------------------------------------------------------"
               "---\n");
        printf("%-10s: %-4u - %s\n", "Expected", expectedResult,
               comparisonStatusToString(expectedResult));
        printf("%-10s: %-4u - %s\n", "Actual", comp,
               comparisonStatusToString(comp));
        printFirstDifference(nodeID1, &doc1, nodeID2, &doc2);
        printf("---------------------------------------------------------------"
               "---\n");
    }

    destroyDocument(&doc1);
    destroyDocument(&doc2);

    destroyGlobals();

    return result;
}

void testAndCount(const char *fileLocation1, const char *fileLocation2,
                  const ComparisonStatus expectedResult, const char *testName,
                  size_t *localSuccesses, size_t *localFailures) {
    if (compareFiles(fileLocation1, fileLocation2, expectedResult, testName) ==
        TEST_SUCCESS) {
        (*localSuccesses)++;
    } else {
        (*localFailures)++;
    }
}

unsigned char testComparisons(size_t *successes, size_t *failures) {
    printf("Testing document comparisons...\n");
    size_t localSuccesses = 0;
    size_t localFailures = 0;

    testAndCount(TEST_FILE_1, TEST_FILE_1, COMPARISON_SUCCESS, "same file",
                 &localSuccesses, &localFailures);
    testAndCount(TEST_FILE_1, TEST_FILE_2, COMPARISON_DIFFERENT_PROPERTIES,
                 "different key-value property", &localSuccesses,
                 &localFailures);
    testAndCount(TEST_FILE_1, TEST_FILE_3, COMPARISON_DIFFERENT_SIZES,
                 "empty file", &localSuccesses, &localFailures);
    testAndCount(TEST_FILE_1, TEST_FILE_4, COMPARISON_MISSING_PROPERTIES,
                 "missing boolean property", &localSuccesses, &localFailures);
    testAndCount(TEST_FILE_1, TEST_FILE_5, COMPARISON_MISSING_PROPERTIES,
                 "missing key-value property", &localSuccesses, &localFailures);
    testAndCount(TEST_FILE_1, TEST_FILE_6, COMPARISON_DIFFERENT_NODE_TYPE,
                 "text node and document node", &localSuccesses,
                 &localFailures);
    testAndCount(TEST_FILE_1, TEST_FILE_7, COMPARISON_DIFFERENT_SIZES,
                 "different sizes", &localSuccesses, &localFailures);
    testAndCount(TEST_FILE_1, TEST_FILE_8, COMPARISON_DIFFERENT_TAGS,
                 "different tags", &localSuccesses, &localFailures);
    testAndCount(TEST_FILE_1, TEST_FILE_9, COMPARISON_DIFFERENT_TEXT,
                 "different text nodes", &localSuccesses, &localFailures);
    testAndCount(TEST_FILE_1, TEST_FILE_10, COMPARISON_SUCCESS,
                 "comments; comments inside text nodes still BUGGED",
                 &localSuccesses, &localFailures);

    printf("[ %zu / %lu ]\n", localSuccesses, localFailures + localSuccesses);

    *successes += localSuccesses;
    *failures += localFailures;

    return localFailures > 0;
}
