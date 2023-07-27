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

    destroyDocument(&doc1);
    destroyDocument(&doc2);

    destroyGlobals();

    if (comp == expectedResult) {
        printf("%s%-20s\n%s", ANSI_COLOR_GREEN, "Success", ANSI_COLOR_RESET);
        return TEST_SUCCESS;
    }

    printf("%s%-20s\n%s", ANSI_COLOR_RED, "Failure", ANSI_COLOR_RESET);
    printFirstDifference(nodeID1, &doc1, nodeID2, &doc2);
    return TEST_FAILURE;
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
                 "different properties", &localSuccesses, &localFailures);
    //    testAndCount(TEST_FILE_3, TEST_FILE_3, COMPARISON_SUCCESS, "empty
    //    files",
    //                 &localSuccesses, &localFailures);

    printf("[ %zu / %lu ]\n", localSuccesses, localFailures + localSuccesses);

    *successes += localSuccesses;
    *failures += localFailures;

    return localFailures > 0;
}
