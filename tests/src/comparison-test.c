#include "log.h"
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
        FLO_LOG_TEST_FAILED {
            FLO_ERROR("Failed to created actual DOM from file ");
            FLO_ERROR(startFileLocation, FLO_NEWLINE);
        }
        return test;
    }

    test.expected = flo_html_createDomFromFile(expectedFileLocation, perm);
    if (test.expected == NULL) {
        FLO_LOG_TEST_FAILED {
            FLO_ERROR("Failed to created expected DOM from file ");
            FLO_ERROR(expectedFileLocation, FLO_NEWLINE);
        }
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
        FLO_LOG_TEST_FAILED {
            printTestResultDifferenceErrorCode(
                QUERY_SUCCESS, flo_html_queryingStatusToString(QUERY_SUCCESS),
                queryStatus, flo_html_queryingStatusToString(queryStatus));
        }

        return TEST_FAILURE;
    }

    return TEST_SUCCESS;
}

TestStatus compareWithCodeAndEndTest(ComparisonTest *comparisonTest,
                                     flo_html_ComparisonStatus expectedStatus,
                                     flo_Arena scratch) {
    TestStatus result = TEST_FAILURE;

    flo_html_ComparisonResult comp = flo_html_equals(
        comparisonTest->actual, comparisonTest->expected, scratch);

    if (comp.status == expectedStatus) {
        printTestSuccess();
        result = TEST_SUCCESS;
    } else {
        FLO_LOG_TEST_FAILED {
            printTestResultDifferenceErrorCode(
                expectedStatus,
                flo_html_comparisonStatusToString(expectedStatus), comp.status,
                flo_html_comparisonStatusToString(comp.status));
            flo_html_printFirstDifference(comp.nodeID1, comparisonTest->actual,
                                          comp.nodeID2,
                                          comparisonTest->expected, scratch);
        }
    }
    return result;
}

TestStatus compareAndEndTest(ComparisonTest *comparisonTest,
                             flo_Arena scratch) {
    return compareWithCodeAndEndTest(comparisonTest, COMPARISON_SUCCESS,
                                     scratch);
}
