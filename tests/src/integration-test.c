#include <flo/html-parser.h>
#include <flo/html-parser/util/memory.h>

#include "comparison-test.h"
#include "integration-test.h"
#include "test-status.h"
#include "test.h"

#define CURRENT_DIR "tests/src/inputs/"
#define PARSE_QUERY_MODIFY_BEFORE CURRENT_DIR "parse-query-modify-before.html"
#define PARSE_QUERY_MODIFY_AFTER CURRENT_DIR "parse-query-modify-after.html"

static TestStatus parseQueryModify(flo_html_Arena scratch) {
    printTestStart("Parse/Query/Modify");

    ComparisonTest comparisonTest =
        initComparisonTest(FLO_HTML_S(PARSE_QUERY_MODIFY_BEFORE),
                           FLO_HTML_S(PARSE_QUERY_MODIFY_AFTER), &scratch);

    flo_html_node_id_a results;
    flo_html_QueryStatus actual = flo_html_querySelectorAll(
        FLO_HTML_S("title"), comparisonTest.actual, &results, &scratch);

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
    flo_html_setTextContent(results.buf[0], FLO_HTML_S("FOURTH"),
                            comparisonTest.actual);
    flo_html_addBooleanPropertyToNode(results.buf[0], FLO_HTML_S("the-fourth"),
                                      comparisonTest.actual);
    flo_html_addPropertyToNode(results.buf[0], FLO_HTML_S("the-property"),
                               FLO_HTML_S("my value"), comparisonTest.actual);

    flo_html_node_id currentNodeID = 0;
    actual = flo_html_querySelector(FLO_HTML_S("head"), comparisonTest.actual,
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

    flo_html_prependHTMLFromString(
        currentNodeID,
        FLO_HTML_S("<title "
                   "id=\"first-title-tag\"></title><title>FIRST</"
                   "title><title>SECOND</title><title>THIRD</title>"),
        comparisonTest.actual, &scratch);

    actual = flo_html_querySelectorAll(
        FLO_HTML_S("title"), comparisonTest.actual, &results, &scratch);
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
        flo_html_querySelector(FLO_HTML_S("#first-title-tag"),
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

    flo_html_removeNode(currentNodeID, comparisonTest.actual.dom);

    actual = flo_html_querySelectorAll(
        FLO_HTML_S("title"), comparisonTest.actual, &results, &scratch);
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

    flo_html_writeHTMLToFile(
        comparisonTest.actual,
        FLO_HTML_S("boing-boing/bang-bang/boop/boop/result.html"));

    return compareAndEndTest(&comparisonTest, scratch);
}

bool testIntegrations(ptrdiff_t *successes, ptrdiff_t *failures,
                      flo_html_Arena scratch) {
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
