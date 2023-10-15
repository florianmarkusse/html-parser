#include <flo/html-parser.h>
#include <flo/html-parser/util/memory.h>
#include <stdio.h>
#include <stdlib.h>

#include "dom/querying/querying.h"
#include "test-status.h"
#include "test.h"

#define CURRENT_DIR "tests/src/dom/querying/inputs/"
#define TEST_FILE_1 CURRENT_DIR "test-1.html"

static const TestFile testFiles[] = {
    {TEST_FILE_1, FLO_HTML_S("body div p h1 lalalal input"),
     QUERY_NOT_SEEN_BEFORE, 0, "unknown tag"},
    {TEST_FILE_1, FLO_HTML_S("[html-new]"), QUERY_NOT_SEEN_BEFORE, 0,
     "unknown attribute"},
    {TEST_FILE_1, FLO_HTML_S("[html]"), QUERY_SUCCESS, 2,
     "with html attribute"},
    {TEST_FILE_1, FLO_HTML_S("body"), QUERY_SUCCESS, 7, "single tag selector"},
    {TEST_FILE_1, FLO_HTML_S("body head"), QUERY_SUCCESS, 0, "no nodes found"},
};

static const ptrdiff_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus testQuery(const flo_html_String fileLocation,
                            const flo_html_String cssQuery,
                            const flo_html_QueryStatus expectedStatus,
                            const flo_html_node_id expectedNode,
                            flo_html_Arena scratch) {
    flo_html_ParsedHTML parsed;
    if (flo_html_fromFile(fileLocation, &parsed, &scratch) != USER_SUCCESS) {
        FLO_HTML_PRINT_ERROR(
            "Failed to created DOM & TextStore from file %.*s\n",
            FLO_HTML_S_P(fileLocation));
        return TEST_ERROR_INITIALIZATION;
    }

    flo_html_node_id actualNode = 0;
    flo_html_QueryStatus actual =
        flo_html_querySelector(cssQuery, parsed, &actualNode, scratch);

    TestStatus result = TEST_FAILURE;

    if (actual == expectedStatus &&
        (expectedStatus != QUERY_SUCCESS || actualNode == expectedNode)) {
        printTestSuccess();
        result = TEST_SUCCESS;
    } else {
        printTestFailure();
        printTestDemarcation();
        if (actual != expectedStatus) {
            printTestResultDifferenceErrorCode(
                expectedStatus, flo_html_queryingStatusToString(expectedStatus),
                actual, flo_html_queryingStatusToString(actual));
        } else {
            printTestResultDifferenceNumber(expectedNode, actualNode);
        }

        printTestDemarcation();
    }

    return result;
}

unsigned char testQuerySelector(ptrdiff_t *successes, ptrdiff_t *failures,
                                flo_html_Arena scratch) {
    printTestTopicStart("querySelector");
    ptrdiff_t localSuccesses = 0;
    ptrdiff_t localFailures = 0;

    for (ptrdiff_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];

        printTestStart(testFile.testName);

        if (testQuery(FLO_HTML_S_LEN(testFile.fileLocation,
                                     strlen(testFile.fileLocation)),
                      testFile.cssQuery, testFile.expectedStatus,
                      testFile.expectedResult, scratch) != TEST_SUCCESS) {
            localFailures++;
        } else {
            localSuccesses++;
        }
    }

    printTestScore(localSuccesses, localFailures);

    *successes += localSuccesses;
    *failures += localFailures;

    return localFailures > 0;
}
