#include <flo/html-parser.h>
#include <flo/html-parser/type/element/elements-print.h>
#include <flo/html-parser/utils/memory/memory.h>

#include "comparison-test.h"
#include "flo/html-parser/dom/query/query-status.h"
#include "test.h"

TestStatus initComparisonTest(ComparisonTest *comparisonTest,
                              const flo_html_String startFileLocation,
                              const flo_html_String expectedFileLocation) {
    flo_html_ElementStatus initStatus =
        flo_html_createTextStore(&comparisonTest->startTextStore);
    if (initStatus != ELEMENT_SUCCESS) {
        return failWithMessageAndCode(
            FLO_HTML_S("Failed to initialize start text store!\n"),
            TEST_ERROR_INITIALIZATION, comparisonTest);
    }
    if (flo_html_createDomFromFile(
            startFileLocation, &comparisonTest->startflo_html_Dom,
            &comparisonTest->startTextStore) != DOM_SUCCESS) {
        printf("IN HERE\n");
        return failWithMessageAndCode(
            FLO_HTML_S("Failed to create start DOM from file!\n"),
            TEST_ERROR_INITIALIZATION, comparisonTest);
    }
    initStatus = flo_html_createTextStore(&comparisonTest->expectedTextStore);
    if (initStatus != ELEMENT_SUCCESS) {
        return failWithMessageAndCode(
            FLO_HTML_S("Failed to initialize expected text store!\n"),
            TEST_ERROR_INITIALIZATION, comparisonTest);
    }
    if (flo_html_createDomFromFile(
            expectedFileLocation, &comparisonTest->expectedflo_html_Dom,
            &comparisonTest->expectedTextStore) != DOM_SUCCESS) {
        return failWithMessageAndCode(
            FLO_HTML_S("Failed to create expected DOM from file!\n"),
            TEST_ERROR_INITIALIZATION, comparisonTest);
    }

    return TEST_SUCCESS;
}

TestStatus getNodeFromQuerySelector(const flo_html_String cssQuery,
                                    ComparisonTest *comparisonTest,
                                    flo_html_node_id *foundNode) {
    flo_html_QueryStatus queryStatus =
        flo_html_querySelector(cssQuery, &comparisonTest->startflo_html_Dom,
                               &comparisonTest->startTextStore, foundNode);

    if (queryStatus != QUERY_SUCCESS) {
        destroyComparisonTest(comparisonTest);

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
                                  const TestStatus failureStatus,
                                  ComparisonTest *comparisonTest) {
    destroyComparisonTest(comparisonTest);

    printTestFailure();
    printTestDemarcation();
    printf("%s", failureMessage.buf);
    printTestDemarcation();

    if (failureStatus == TEST_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Improper use of failWithMessageAndCode!\n");
        return TEST_FAILURE;
    }
    return failureStatus;
}

TestStatus failWithMessage(const flo_html_String failureMessage,
                           ComparisonTest *comparisonTest) {
    return failWithMessageAndCode(failureMessage, TEST_FAILURE, comparisonTest);
}

TestStatus
compareWithCodeAndEndTest(ComparisonTest *comparisonTest,
                          const flo_html_ComparisonStatus expectedStatus) {
    TestStatus result = TEST_FAILURE;

    flo_html_node_id nodeID1 = 0;
    flo_html_node_id nodeID2 = 0;
    flo_html_ComparisonStatus comp =
        flo_html_equalsWithNode(&nodeID1, &comparisonTest->startflo_html_Dom,
                                &comparisonTest->startTextStore, &nodeID2,
                                &comparisonTest->expectedflo_html_Dom,
                                &comparisonTest->expectedTextStore);

    if (comp == expectedStatus) {
        printTestSuccess();
        result = TEST_SUCCESS;
    } else {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceErrorCode(
            expectedStatus, flo_html_comparisonStatusToString(expectedStatus),
            comp, flo_html_comparisonStatusToString(comp));
        flo_html_printFirstDifference(nodeID1,
                                      &comparisonTest->startflo_html_Dom,
                                      &comparisonTest->startTextStore, nodeID2,
                                      &comparisonTest->expectedflo_html_Dom,
                                      &comparisonTest->expectedTextStore);
        printTestDemarcation();
    }

    destroyComparisonTest(comparisonTest);
    return result;
}

TestStatus compareAndEndTest(ComparisonTest *comparisonTest) {
    return compareWithCodeAndEndTest(comparisonTest, COMPARISON_SUCCESS);
}

void destroyComparisonTest(ComparisonTest *comparisonTest) {
    flo_html_destroyDom(&comparisonTest->startflo_html_Dom);
    flo_html_destroyTextStore(&comparisonTest->startTextStore);
    flo_html_destroyDom(&comparisonTest->expectedflo_html_Dom);
    flo_html_destroyTextStore(&comparisonTest->expectedTextStore);
}
