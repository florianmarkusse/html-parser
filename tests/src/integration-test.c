#include <flo/html-parser.h>
#include <memory/arena.h>

#include "comparison-test.h"
#include "integration-test.h"
#include "test-status.h"
#include "test.h"

#define CURRENT_DIR "tests/src/inputs/"
#define PARSE_QUERY_MODIFY_BEFORE CURRENT_DIR "parse-query-modify-before.html"
#define PARSE_QUERY_MODIFY_AFTER CURRENT_DIR "parse-query-modify-after.html"

static TestStatus parseQueryModify(flo_Arena scratch) {
    printTestStart("Parse/Query/Modify");

    ComparisonTest comparisonTest =
        initComparisonTest(FLO_STRING(PARSE_QUERY_MODIFY_BEFORE),
                           FLO_STRING(PARSE_QUERY_MODIFY_AFTER), &scratch);

    flo_html_node_id_a results;
    flo_html_QueryStatus actual = flo_html_querySelectorAll(
        FLO_STRING("title"), comparisonTest.actual, &results, &scratch);

    if (actual != QUERY_SUCCESS) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceErrorCode(
            QUERY_SUCCESS, flo_html_queryingStatusToString(QUERY_SUCCESS),
            actual, flo_html_queryingStatusToString(actual));
        printTestDemarcation();
        return TEST_FAILURE;
    }

    if (results.len != 1) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceNumber(1, results.len);
        printf("Node IDs received...\n");
        for (ptrdiff_t i = 0; i < results.len; i++) {
            printf("%u\n", results.buf[i]);
        }
        printTestDemarcation();
        return TEST_FAILURE;
    }
    flo_html_setTextContent(results.buf[0], FLO_STRING("FOURTH"),
                            comparisonTest.actual, &scratch);
    flo_html_addBooleanPropertyToNode(results.buf[0], FLO_STRING("the-fourth"),
                                      comparisonTest.actual, &scratch);
    flo_html_addPropertyToNode(results.buf[0], FLO_STRING("the-property"),
                               FLO_STRING("my value"), comparisonTest.actual,
                               &scratch);

    flo_html_node_id currentNodeID = 0;
    actual = flo_html_querySelector(FLO_STRING("head"), comparisonTest.actual,
                                    &currentNodeID, scratch);
    if (actual != QUERY_SUCCESS) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceErrorCode(
            QUERY_SUCCESS, flo_html_queryingStatusToString(QUERY_SUCCESS),
            actual, flo_html_queryingStatusToString(actual));
        printTestDemarcation();
        return TEST_FAILURE;
    }

    if (flo_html_prependHTMLFromString(
            currentNodeID,
            FLO_STRING("<title "
                       "id=\"first-title-tag\"></title><title>FIRST</"
                       "title><title>SECOND</title><title>THIRD</title>"),
            comparisonTest.actual, &scratch) == 0) {
        printf("Failed to prepend HTML from string.\n");
        return TEST_FAILURE;
    }

    actual = flo_html_querySelectorAll(
        FLO_STRING("title"), comparisonTest.actual, &results, &scratch);
    if (actual != QUERY_SUCCESS) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceErrorCode(
            QUERY_SUCCESS, flo_html_queryingStatusToString(QUERY_SUCCESS),
            actual, flo_html_queryingStatusToString(actual));
        printTestDemarcation();
        return TEST_FAILURE;
    }

    if (results.len != 5) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceNumber(5, results.len);
        printf("Node IDs received...\n");
        for (ptrdiff_t i = 0; i < results.len; i++) {
            printf("%u\n", results.buf[i]);
        }
        printTestDemarcation();
        return TEST_FAILURE;
    }

    actual =
        flo_html_querySelector(FLO_STRING("#first-title-tag"),
                               comparisonTest.actual, &currentNodeID, scratch);
    if (actual != QUERY_SUCCESS) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceErrorCode(
            QUERY_SUCCESS, flo_html_queryingStatusToString(QUERY_SUCCESS),
            actual, flo_html_queryingStatusToString(actual));
        printTestDemarcation();
        return TEST_FAILURE;
    }

    flo_html_removeNode(currentNodeID, comparisonTest.actual);

    actual = flo_html_querySelectorAll(
        FLO_STRING("title"), comparisonTest.actual, &results, &scratch);
    if (actual != QUERY_SUCCESS) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceErrorCode(
            QUERY_SUCCESS, flo_html_queryingStatusToString(QUERY_SUCCESS),
            actual, flo_html_queryingStatusToString(actual));
        printTestDemarcation();
        return TEST_FAILURE;
    }

    if (results.len != 4) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceNumber(4, results.len);
        printf("Node IDs received...\n");
        for (ptrdiff_t i = 0; i < results.len; i++) {
            printf("%u\n", results.buf[i]);
        }
        printTestDemarcation();
        return TEST_FAILURE;
    }

    flo_html_printHTML(comparisonTest.actual);

    flo_html_Dom *duplicatedDom =
        flo_html_duplicateDom(comparisonTest.actual, &scratch);

    flo_html_ComparisonResult comp =
        flo_html_equals(comparisonTest.actual, duplicatedDom, scratch);
    if (comp.status != COMPARISON_SUCCESS) {
        printTestFailure();
        printTestDemarcation();
        printf("Duplication failed!\n");
        printTestDemarcation();
        return TEST_FAILURE;
    }

    return compareAndEndTest(&comparisonTest, scratch);
}

bool testIntegrations(ptrdiff_t *successes, ptrdiff_t *failures,
                      flo_Arena scratch) {
    printTestTopicStart("Integration tests");

    ptrdiff_t localSuccesses = 0;
    ptrdiff_t localFailures = 0;

    if (parseQueryModify(scratch) != TEST_SUCCESS) {
        localFailures++;
    } else {
        localSuccesses++;
    }

    printTestScore(localSuccesses, localFailures);

    *successes += localSuccesses;
    *failures += localFailures;

    return localFailures > 0;
}
