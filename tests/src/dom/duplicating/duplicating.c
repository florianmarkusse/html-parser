#include "log.h"
#include <flo/html-parser.h>
#include <stdio.h>
#include <stdlib.h>

#include "dom/duplicating/duplicating.h"
#include "flo/html-parser/dom/query/query.h"
#include "test-status.h"
#include "test.h"

#define CURRENT_DIR "tests/src/dom/duplicating/inputs/"
#define TEST_FILE_1 CURRENT_DIR "test-1.html"

static TestStatus testDuplication(flo_Arena scratch) {
    flo_String testFileLocation =
        FLO_STRING_LEN(TEST_FILE_1, strlen(TEST_FILE_1));

    flo_html_Dom *dom = flo_html_createDomFromFile(testFileLocation, &scratch);
    if (dom == NULL) {
        FLO_FLUSH_AFTER(FLO_STDERR) {
            FLO_ERROR("Failed to created DOM from file ");
            FLO_ERROR(testFileLocation, FLO_NEWLINE);
        }
        printTestFailure();
        return TEST_ERROR_INITIALIZATION;
    }

    flo_html_Dom *duplicatedDom = flo_html_duplicateDom(dom, &scratch);

    flo_html_ComparisonResult comp =
        flo_html_equals(dom, duplicatedDom, scratch);
    if (comp.status != COMPARISON_SUCCESS) {
        printTestFailure();
        printTestDemarcation();
        printf("Duplication failed!\n");
        printTestDemarcation();
        return TEST_FAILURE;
    }

    flo_html_node_id nodeID = 0;
    flo_html_QueryStatus status = flo_html_querySelector(
        FLO_STRING("[special-one]"), dom, &nodeID, scratch);
    if (status != QUERY_SUCCESS) {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifferenceErrorCode(
            QUERY_SUCCESS, flo_html_queryingStatusToString(QUERY_SUCCESS),
            status, flo_html_queryingStatusToString(status));
        printTestDemarcation();
        return TEST_FAILURE;
    }

    flo_html_removeBooleanProperty(nodeID, FLO_STRING("special-one"), dom);

    comp = flo_html_equals(dom, duplicatedDom, scratch);
    if (comp.status != COMPARISON_DIFFERENT_SIZES) {
        printTestFailure();
        printTestDemarcation();
        printf("Did not recognize different content !\n");
        printf(
            "Returned %.*s\n",
            FLO_STRING_PRINT(flo_html_comparisonStatusToString(comp.status)));
        printTestDemarcation();
        return TEST_FAILURE;
    }
    printf("Returned %.*s\n",
           FLO_STRING_PRINT(flo_html_comparisonStatusToString(comp.status)));

    printTestSuccess();

    return TEST_SUCCESS;
}

bool testflo_html_DomDuplications(ptrdiff_t *successes, ptrdiff_t *failures,
                                  flo_Arena scratch) {
    printTestTopicStart("Duplication test");
    ptrdiff_t localSuccesses = 0;
    ptrdiff_t localFailures = 0;

    printTestStart("simple duplication");
    if (testDuplication(scratch) != TEST_SUCCESS) {
        localFailures++;
    } else {
        localSuccesses++;
    }

    printTestScore(localSuccesses, localFailures);

    *successes += localSuccesses;
    *failures += localFailures;

    return localFailures > 0;
}
