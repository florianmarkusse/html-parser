#include <flo/html-parser.h>
#include <flo/html-parser/util//memory.h>

#include "comparison-test.h"
#include "integration-test.h"
#include "test-status.h"
#include "test.h"

#define CURRENT_DIR "tests/src/inputs/"
#define PARSE_QUERY_MODIFY_BEFORE CURRENT_DIR "parse-query-modify-before.html"
#define PARSE_QUERY_MODIFY_AFTER CURRENT_DIR "parse-query-modify-after.html"

static TestStatus parseQueryModify() {
    printTestStart("Parse/Query/Modify");
    TestStatus result = TEST_FAILURE;

    ComparisonTest comparisonTest;
    result = initComparisonTest(&comparisonTest,
                                FLO_HTML_S(PARSE_QUERY_MODIFY_BEFORE),
                                FLO_HTML_S(PARSE_QUERY_MODIFY_AFTER));
    if (result != TEST_SUCCESS) {
        return result;
    }

    flo_html_node_id *results = NULL;
    ptrdiff_t resultsLen = 0;
    flo_html_QueryStatus actual = flo_html_querySelectorAll(
        FLO_HTML_S("title"), &comparisonTest.startflo_html_Dom,
        &comparisonTest.startTextStore, &results, &resultsLen);

    if (actual != QUERY_SUCCESS) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceErrorCode(
            QUERY_SUCCESS, flo_html_queryingStatusToString(QUERY_SUCCESS),
            actual, flo_html_queryingStatusToString(actual));
        printTestDemarcation();
        return TEST_FAILURE;
    }

    if (resultsLen != 1) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceNumber(1, resultsLen);
        printf("Node IDs received...\n");
        for (ptrdiff_t i = 0; i < resultsLen; i++) {
            printf("%u\n", results[i]);
        }
        printTestDemarcation();
        FLO_HTML_FREE_TO_NULL(results);
        return TEST_FAILURE;
    }
    flo_html_setTextContent(results[0], FLO_HTML_S("FOURTH"),
                            &comparisonTest.startflo_html_Dom,
                            &comparisonTest.startTextStore);
    flo_html_addBooleanPropertyToNode(results[0], FLO_HTML_S("the-fourth"),
                                      &comparisonTest.startflo_html_Dom,
                                      &comparisonTest.startTextStore);
    flo_html_addPropertyToNode(
        results[0], FLO_HTML_S("the-property"), FLO_HTML_S("my value"),
        &comparisonTest.startflo_html_Dom, &comparisonTest.startTextStore);
    FLO_HTML_FREE_TO_NULL(results);

    flo_html_node_id currentNodeID = 0;
    actual = flo_html_querySelector(
        FLO_HTML_S("head"), &comparisonTest.startflo_html_Dom,
        &comparisonTest.startTextStore, &currentNodeID);
    if (actual != QUERY_SUCCESS) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceErrorCode(
            QUERY_SUCCESS, flo_html_queryingStatusToString(QUERY_SUCCESS),
            actual, flo_html_queryingStatusToString(actual));
        printTestDemarcation();
        return TEST_FAILURE;
    }

    flo_html_prependHTMLFromString(
        currentNodeID,
        FLO_HTML_S("<title "
                   "id=\"first-title-tag\"></title><title>FIRST</"
                   "title><title>SECOND</title><title>THIRD</title>"),
        &comparisonTest.startflo_html_Dom, &comparisonTest.startTextStore);

    actual = flo_html_querySelectorAll(
        FLO_HTML_S("title"), &comparisonTest.startflo_html_Dom,
        &comparisonTest.startTextStore, &results, &resultsLen);
    if (actual != QUERY_SUCCESS) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceErrorCode(
            QUERY_SUCCESS, flo_html_queryingStatusToString(QUERY_SUCCESS),
            actual, flo_html_queryingStatusToString(actual));
        printTestDemarcation();
        FLO_HTML_FREE_TO_NULL(results);
        return TEST_FAILURE;
    }

    if (resultsLen != 5) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceNumber(5, resultsLen);
        printf("Node IDs received...\n");
        for (ptrdiff_t i = 0; i < resultsLen; i++) {
            printf("%u\n", results[i]);
        }
        printTestDemarcation();
        FLO_HTML_FREE_TO_NULL(results);
        return TEST_FAILURE;
    }

    FLO_HTML_FREE_TO_NULL(results);

    actual = flo_html_querySelector(
        FLO_HTML_S("#first-title-tag"), &comparisonTest.startflo_html_Dom,
        &comparisonTest.startTextStore, &currentNodeID);
    if (actual != QUERY_SUCCESS) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceErrorCode(
            QUERY_SUCCESS, flo_html_queryingStatusToString(QUERY_SUCCESS),
            actual, flo_html_queryingStatusToString(actual));
        printTestDemarcation();
        FLO_HTML_FREE_TO_NULL(results);
        return TEST_FAILURE;
    }

    flo_html_removeNode(currentNodeID, &comparisonTest.startflo_html_Dom);

    actual = flo_html_querySelectorAll(
        FLO_HTML_S("title"), &comparisonTest.startflo_html_Dom,
        &comparisonTest.startTextStore, &results, &resultsLen);
    if (actual != QUERY_SUCCESS) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceErrorCode(
            QUERY_SUCCESS, flo_html_queryingStatusToString(QUERY_SUCCESS),
            actual, flo_html_queryingStatusToString(actual));
        printTestDemarcation();
        FLO_HTML_FREE_TO_NULL(results);
        return TEST_FAILURE;
    }

    if (resultsLen != 4) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceNumber(4, resultsLen);
        printf("Node IDs received...\n");
        for (ptrdiff_t i = 0; i < resultsLen; i++) {
            printf("%u\n", results[i]);
        }
        printTestDemarcation();
        FLO_HTML_FREE_TO_NULL(results);
        return TEST_FAILURE;
    }

    FLO_HTML_FREE_TO_NULL(results);

    return compareAndEndTest(&comparisonTest);
}

bool testIntegrations(ptrdiff_t *successes, ptrdiff_t *failures) {
    printTestTopicStart("Integration tests");

    ptrdiff_t localSuccesses = 0;
    ptrdiff_t localFailures = 0;

    if (parseQueryModify() != TEST_SUCCESS) {
        localFailures++;
    } else {
        localSuccesses++;
    }

    printTestScore(localSuccesses, localFailures);

    *successes += localSuccesses;
    *failures += localFailures;

    return localFailures > 0;
}
