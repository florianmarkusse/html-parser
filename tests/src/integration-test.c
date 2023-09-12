
#include <flo/html-parser.h>
#include <flo/html-parser/utils/memory/memory.h>

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
    result = initComparisonTest(&comparisonTest, PARSE_QUERY_MODIFY_BEFORE,
                                PARSE_QUERY_MODIFY_AFTER);
    if (result != TEST_SUCCESS) {
        return result;
    }

    node_id *results = NULL;
    size_t resultsLen = 0;
    QueryStatus actual = querySelectorAll("title", &comparisonTest.startDom,
                                          &comparisonTest.startDataContainer,
                                          &results, &resultsLen);

    if (actual != QUERY_SUCCESS) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceErrorCode(
            QUERY_SUCCESS, queryingStatusToString(QUERY_SUCCESS), actual,
            queryingStatusToString(actual));
        printTestDemarcation();
        return TEST_FAILURE;
    }

    if (resultsLen != 1) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceNumber(1, resultsLen);
        printf("Node IDs received...\n");
        for (size_t i = 0; i < resultsLen; i++) {
            printf("%u\n", results[i]);
        }
        printTestDemarcation();
        FREE_TO_NULL(results);
        return TEST_FAILURE;
    }
    setTextContent(results[0], "FOURTH", &comparisonTest.startDom,
                   &comparisonTest.startDataContainer);
    addBooleanPropertyToNodeString(results[0], "the-fourth",
                                   &comparisonTest.startDom,
                                   &comparisonTest.startDataContainer);
    addPropertyToNodeStrings(results[0], "the-property", "my value",
                             &comparisonTest.startDom,
                             &comparisonTest.startDataContainer);
    FREE_TO_NULL(results);

    node_id currentNodeID = 0;
    actual = querySelector("head", &comparisonTest.startDom,
                           &comparisonTest.startDataContainer, &currentNodeID);
    if (actual != QUERY_SUCCESS) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceErrorCode(
            QUERY_SUCCESS, queryingStatusToString(QUERY_SUCCESS), actual,
            queryingStatusToString(actual));
        printTestDemarcation();
        return TEST_FAILURE;
    }

    prependHTMLFromString(currentNodeID,
                          "<title "
                          "id=\"first-title-tag\"></title><title>FIRST</"
                          "title><title>SECOND</title><title>THIRD</title>",
                          &comparisonTest.startDom,
                          &comparisonTest.startDataContainer);

    actual = querySelectorAll("title", &comparisonTest.startDom,
                              &comparisonTest.startDataContainer, &results,
                              &resultsLen);
    if (actual != QUERY_SUCCESS) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceErrorCode(
            QUERY_SUCCESS, queryingStatusToString(QUERY_SUCCESS), actual,
            queryingStatusToString(actual));
        printTestDemarcation();
        FREE_TO_NULL(results);
        return TEST_FAILURE;
    }

    if (resultsLen != 5) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceNumber(5, resultsLen);
        printf("Node IDs received...\n");
        for (size_t i = 0; i < resultsLen; i++) {
            printf("%u\n", results[i]);
        }
        printTestDemarcation();
        FREE_TO_NULL(results);
        return TEST_FAILURE;
    }

    FREE_TO_NULL(results);

    actual = querySelector("#first-title-tag", &comparisonTest.startDom,
                           &comparisonTest.startDataContainer, &currentNodeID);
    if (actual != QUERY_SUCCESS) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceErrorCode(
            QUERY_SUCCESS, queryingStatusToString(QUERY_SUCCESS), actual,
            queryingStatusToString(actual));
        printTestDemarcation();
        FREE_TO_NULL(results);
        return TEST_FAILURE;
    }

    removeNode(currentNodeID, &comparisonTest.startDom);

    actual = querySelectorAll("title", &comparisonTest.startDom,
                              &comparisonTest.startDataContainer, &results,
                              &resultsLen);
    if (actual != QUERY_SUCCESS) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceErrorCode(
            QUERY_SUCCESS, queryingStatusToString(QUERY_SUCCESS), actual,
            queryingStatusToString(actual));
        printTestDemarcation();
        FREE_TO_NULL(results);
        return TEST_FAILURE;
    }

    if (resultsLen != 4) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceNumber(4, resultsLen);
        printf("Node IDs received...\n");
        for (size_t i = 0; i < resultsLen; i++) {
            printf("%u\n", results[i]);
        }
        printTestDemarcation();
        FREE_TO_NULL(results);
        return TEST_FAILURE;
    }

    FREE_TO_NULL(results);

    return compareAndEndTest(&comparisonTest);
}

bool testIntegrations(size_t *successes, size_t *failures) {
    printTestTopicStart("Integration tests");

    size_t localSuccesses = 0;
    size_t localFailures = 0;

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
