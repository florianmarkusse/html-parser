#include <flo/html-parser.h>
#include <flo/html-parser/utils/memory/memory.h>
#include <stdio.h>
#include <stdlib.h>

#include "dom/querying/querying.h"
#include "test-status.h"
#include "test.h"

#define CURRENT_DIR "tests/src/dom/querying/inputs/"
#define TEST_FILE_1 CURRENT_DIR "test-1.html"

static const TestFile testFiles[] = {
    {TEST_FILE_1, "body div p h1 lalalal input", QUERY_NOT_SEEN_BEFORE, 0,
     "unknown tag"},
    {TEST_FILE_1, "[html-new]", QUERY_NOT_SEEN_BEFORE, 0, "unknown attribute"},
    {TEST_FILE_1, "[html]", QUERY_SUCCESS, 1, "with html attribute"},
    {TEST_FILE_1, "body", QUERY_SUCCESS, 6, "single tag selector"},
    {TEST_FILE_1, "body head", QUERY_SUCCESS, 0, "no nodes found"},
};

static const size_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus testQuery(const char *fileLocation, const char *cssQuery,
                            const flo_html_QueryStatus expectedStatus,
                            const flo_html_node_id expectedNode) {
    flo_html_TextStore textStore;
    flo_html_ElementStatus initStatus = flo_html_createTextStore(&textStore);
    if (initStatus != ELEMENT_SUCCESS) {
        FLO_HTML_ERROR_WITH_CODE_ONLY(
            flo_html_elementStatusToString(initStatus),
            "Failed to initialize text store");
        return TEST_ERROR_INITIALIZATION;
    }

    flo_html_Dom dom;
    if (createflo_html_DomFromFile(fileLocation, &dom, &textStore) !=
        DOM_SUCCESS) {
        flo_html_destroyTextStore(&textStore);
        return TEST_ERROR_INITIALIZATION;
    }

    flo_html_node_id actualNode = 0;
    flo_html_QueryStatus actual =
        flo_html_querySelector(cssQuery, &dom, &textStore, &actualNode);

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

    destroyflo_html_Dom(&dom);
    flo_html_destroyTextStore(&textStore);

    return result;
}

unsigned char testQuerySelector(size_t *successes, size_t *failures) {
    printTestTopicStart("querySelector");
    size_t localSuccesses = 0;
    size_t localFailures = 0;

    for (size_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];

        printTestStart(testFile.testName);

        if (testQuery(testFile.fileLocation, testFile.cssQuery,
                      testFile.expectedStatus,
                      testFile.expectedResult) != TEST_SUCCESS) {
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
