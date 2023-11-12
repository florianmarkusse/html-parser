#include <flo/html-parser.h>
#include <flo/html-parser/dom/comparison.h>
#include <flo/html-parser/dom/query/query-status.h>
#include <memory/arena.h>

#include "comparison-test.h"
#include "test.h"

ComparisonTest initComparisonTest(flo_String startFileLocation,
                                  flo_String expectedFileLocation,
                                  flo_Arena *perm) {
    ComparisonTest test = {0};
    test.actual = flo_html_createDomFromFile(startFileLocation, perm);
    if (test.actual == NULL) {
        FLO_PRINT_ERROR("Failed to created actual DOM from file %.*s\n",
                             FLO_STRING_PRINT(startFileLocation));
        return test;
    }

    test.expected = flo_html_createDomFromFile(expectedFileLocation, perm);
    if (test.expected == NULL) {
        FLO_PRINT_ERROR("Failed to created expected DOM from file %.*s\n",
                             FLO_STRING_PRINT(expectedFileLocation));
        return test;
    }

    return test;
}

TestStatus getNodeFromQuerySelector(flo_String cssQuery,
                                    ComparisonTest *comparisonTest,
                                    flo_html_node_id *foundNode,
                                    flo_Arena scratch) {
    flo_html_QueryStatus queryStatus = flo_html_querySelector(
        cssQuery, comparisonTest->actual, foundNode, scratch);

    if (queryStatus != QUERY_SUCCESS) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceErrorCode(
            QUERY_SUCCESS, flo_html_queryingStatusToString(QUERY_SUCCESS),
            queryStatus, flo_html_queryingStatusToString(queryStatus));
        printTestDemarcation();

        return TEST_FAILURE;
    }

    return TEST_SUCCESS;
}

TestStatus failWithMessageAndCode(flo_String failureMessage,
                                  TestStatus failureStatus) {
    printTestFailure();
    printTestDemarcation();
    printf("%.*s\n", FLO_STRING_PRINT(failureMessage));
    printTestDemarcation();

    if (failureStatus == TEST_SUCCESS) {
        FLO_PRINT_ERROR("Improper use of failWithMessageAndCode!\n");
        return TEST_FAILURE;
    }
    return failureStatus;
}

TestStatus failWithMessage(flo_String failureMessage) {
    return failWithMessageAndCode(failureMessage, TEST_FAILURE);
}

TestStatus
compareWithCodeAndEndTest(ComparisonTest *comparisonTest,
                          flo_html_ComparisonStatus expectedStatus,
                          flo_Arena scratch) {
    TestStatus result = TEST_FAILURE;

    flo_html_ComparisonResult comp = flo_html_equals(
        comparisonTest->actual, comparisonTest->expected, scratch);

    if (comp.status == expectedStatus) {
        printTestSuccess();
        result = TEST_SUCCESS;
    } else {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceErrorCode(
            expectedStatus, flo_html_comparisonStatusToString(expectedStatus),
            comp.status, flo_html_comparisonStatusToString(comp.status));
        flo_html_printFirstDifference(comp.nodeID1, comparisonTest->actual,
                                      comp.nodeID2, comparisonTest->expected,
                                      scratch);
        printTestDemarcation();
    }
    return result;
}

TestStatus compareAndEndTest(ComparisonTest *comparisonTest,
                             flo_Arena scratch) {
    return compareWithCodeAndEndTest(comparisonTest, COMPARISON_SUCCESS,
                                     scratch);
}
