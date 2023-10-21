#include <flo/html-parser.h>
#include <flo/html-parser/dom/comparison.h>
#include <flo/html-parser/dom/query/query-status.h>
#include <flo/html-parser/type/element/elements-print.h>
#include <flo/html-parser/util/memory.h>

#include "comparison-test.h"
#include "test.h"

ComparisonTest initComparisonTest(const flo_html_String startFileLocation,
                                  const flo_html_String expectedFileLocation,
                                  flo_html_Arena *perm) {
    ComparisonTest test = {0};

    if (flo_html_fromFile(startFileLocation, &test.actual, perm) !=
        USER_SUCCESS) {
        FLO_HTML_PRINT_ERROR(
            "Failed to created DOM & TextStore from file %.*s\n",
            FLO_HTML_S_P(startFileLocation));
        return test;
    }

    if (flo_html_fromFile(expectedFileLocation, &test.expected, perm) !=
        USER_SUCCESS) {
        FLO_HTML_PRINT_ERROR(
            "Failed to created DOM & TextStore from file %.*s\n",
            FLO_HTML_S_P(expectedFileLocation));
        return test;
    }

    return test;
}

TestStatus getNodeFromQuerySelector(const flo_html_String cssQuery,
                                    ComparisonTest *comparisonTest,
                                    flo_html_node_id *foundNode,
                                    flo_html_Arena scratch) {
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

TestStatus failWithMessageAndCode(const flo_html_String failureMessage,
                                  const TestStatus failureStatus) {
    printTestFailure();
    printTestDemarcation();
    printf("%.*s\n", FLO_HTML_S_P(failureMessage));
    printTestDemarcation();

    if (failureStatus == TEST_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Improper use of failWithMessageAndCode!\n");
        return TEST_FAILURE;
    }
    return failureStatus;
}

TestStatus failWithMessage(const flo_html_String failureMessage) {
    return failWithMessageAndCode(failureMessage, TEST_FAILURE);
}

TestStatus
compareWithCodeAndEndTest(ComparisonTest *comparisonTest,
                          const flo_html_ComparisonStatus expectedStatus,
                          flo_html_Arena scratch) {
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
                             flo_html_Arena scratch) {
    return compareWithCodeAndEndTest(comparisonTest, COMPARISON_SUCCESS,
                                     scratch);
}
