#include <flo/html-parser.h>
#include <flo/html-parser/utils/memory/memory.h>

#include "comparison-test.h"
#include "flo/html-parser/dom/query/query-status.h"
#include "test.h"

TestStatus initComparisonTest(ComparisonTest *comparisonTest,
                              const char *startFileLocation,
                              const char *expectedFileLocation) {
    ElementStatus initStatus =
        createTextStore(&comparisonTest->startTextStore);
    if (initStatus != ELEMENT_SUCCESS) {
        return failWithMessageAndCode(
            "Failed to initialize start text store!\n",
            TEST_ERROR_INITIALIZATION, comparisonTest);
    }
    if (createDomFromFile(startFileLocation, &comparisonTest->startDom,
                          &comparisonTest->startTextStore) != DOM_SUCCESS) {
        return failWithMessageAndCode("Failed to create start DOM from file!\n",
                                      TEST_ERROR_INITIALIZATION,
                                      comparisonTest);
    }

    initStatus = createTextStore(&comparisonTest->expectedTextStore);
    if (initStatus != ELEMENT_SUCCESS) {
        return failWithMessageAndCode(
            "Failed to initialize expected text store!\n",
            TEST_ERROR_INITIALIZATION, comparisonTest);
    }
    if (createDomFromFile(expectedFileLocation, &comparisonTest->expectedDom,
                          &comparisonTest->expectedTextStore) !=
        DOM_SUCCESS) {
        return failWithMessageAndCode(
            "Failed to create expected DOM from file!\n",
            TEST_ERROR_INITIALIZATION, comparisonTest);
    }

    return TEST_SUCCESS;
}

TestStatus getNodeFromQuerySelector(const char *cssQuery,
                                    ComparisonTest *comparisonTest,
                                    node_id *foundNode) {
    QueryStatus queryStatus =
        querySelector(cssQuery, &comparisonTest->startDom,
                      &comparisonTest->startTextStore, foundNode);

    if (queryStatus != QUERY_SUCCESS) {
        destroyComparisonTest(comparisonTest);

        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceErrorCode(
            QUERY_SUCCESS, flo_html_queryingStatusToString(QUERY_SUCCESS), queryStatus,
            flo_html_queryingStatusToString(queryStatus));
        printTestDemarcation();

        return TEST_FAILURE;
    }

    return TEST_SUCCESS;
}

TestStatus failWithMessageAndCode(const char *failureMessage,
                                  const TestStatus failureStatus,
                                  ComparisonTest *comparisonTest) {
    destroyComparisonTest(comparisonTest);

    printTestFailure();
    printTestDemarcation();
    printf("%s", failureMessage);
    printTestDemarcation();

    if (failureStatus == TEST_SUCCESS) {
        PRINT_ERROR("Improper use of failWithMessageAndCode!\n");
        return TEST_FAILURE;
    }
    return failureStatus;
}

TestStatus failWithMessage(const char *failureMessage,
                           ComparisonTest *comparisonTest) {
    return failWithMessageAndCode(failureMessage, TEST_FAILURE, comparisonTest);
}

TestStatus compareWithCodeAndEndTest(ComparisonTest *comparisonTest,
                                     const ComparisonStatus expectedStatus) {
    TestStatus result = TEST_FAILURE;

    node_id nodeID1 = 0;
    node_id nodeID2 = 0;
    ComparisonStatus comp = flo_html_equalsWithNode(
        &nodeID1, &comparisonTest->startDom,
        &comparisonTest->startTextStore, &nodeID2,
        &comparisonTest->expectedDom, &comparisonTest->expectedTextStore);

    if (comp == expectedStatus) {
        printTestSuccess();
        result = TEST_SUCCESS;
    } else {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceErrorCode(
            COMPARISON_SUCCESS, comparisonStatusToString(COMPARISON_SUCCESS),
            comp, comparisonStatusToString(comp));
        flo_html_printFirstDifference(nodeID1, &comparisonTest->startDom,
                             &comparisonTest->startTextStore, nodeID2,
                             &comparisonTest->expectedDom,
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
    destroyDom(&comparisonTest->startDom);
    destroyTextStore(&comparisonTest->startTextStore);
    destroyDom(&comparisonTest->expectedDom);
    destroyTextStore(&comparisonTest->expectedTextStore);
}
