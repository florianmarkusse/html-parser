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
    flo_html_Dom *dom = flo_html_createDomFromFile(TEST_FILE_1, &scratch);
    if (dom == NULL) {
        FLO_LOG_TEST_FAILED {
            FLO_ERROR("Failed to created DOM from file ");
            FLO_ERROR(TEST_FILE_1, FLO_NEWLINE);
        }
        return TEST_ERROR_INITIALIZATION;
    }

    flo_html_Dom *duplicatedDom = flo_html_duplicateDom(dom, &scratch);

    flo_html_ComparisonResult comp =
        flo_html_equals(dom, duplicatedDom, scratch);
    if (comp.status != COMPARISON_SUCCESS) {
        FLO_LOG_TEST_FAILED {
            FLO_ERROR((FLO_STRING("Duplication failed!\n")));
        }
        return TEST_FAILURE;
    }

    flo_html_node_id nodeID = 0;
    flo_html_QueryStatus status = flo_html_querySelector(
        FLO_STRING("[special-one]"), dom, &nodeID, scratch);
    if (status != QUERY_SUCCESS) {
        FLO_LOG_TEST_FAILED {
            printTestResultDifferenceErrorCode(
                QUERY_SUCCESS, flo_html_queryingStatusToString(QUERY_SUCCESS),
                status, flo_html_queryingStatusToString(status));
        }
        return TEST_FAILURE;
    }

    flo_html_removeBooleanProperty(nodeID, FLO_STRING("special-one"), dom);

    comp = flo_html_equals(dom, duplicatedDom, scratch);
    if (comp.status != COMPARISON_DIFFERENT_SIZES) {
        FLO_LOG_TEST_FAILED {
            FLO_ERROR((FLO_STRING(
                "Did not recognize different content !\nReturned ")));
            FLO_ERROR(flo_html_comparisonStatusToString(comp.status),
                      FLO_NEWLINE);
        }
        return TEST_FAILURE;
    }

    printTestSuccess();

    return TEST_SUCCESS;
}

bool testflo_html_DomDuplications(ptrdiff_t *successes, ptrdiff_t *failures,
                                  flo_Arena scratch) {
    printTestTopicStart(FLO_STRING("Duplication test"));
    ptrdiff_t localSuccesses = 0;
    ptrdiff_t localFailures = 0;

    printTestStart(FLO_STRING("simple duplication"));
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
