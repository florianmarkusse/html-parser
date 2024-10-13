#include "flo/util/log.h"
#include <flo/html-parser.h>
#include <stdio.h>
#include <stdlib.h>

#include "dom/duplicating/duplicating.h"
#include "expectations.h"
#include "flo/html-parser/dom/query/query.h"
#include "test.h"

#define CURRENT_DIR "tests/src/dom/duplicating/inputs/"
#define TEST_FILE_1 CURRENT_DIR "test-1.html"

static void testDuplication(flo_Arena scratch) {
    flo_html_Dom *dom = flo_html_createDomFromFile(TEST_FILE_1, &scratch);
    if (dom == NULL) {
        FLO_TEST_FAILURE {
            FLO_ERROR("Failed to created DOM from file ");
            FLO_ERROR(TEST_FILE_1, FLO_NEWLINE);
        }
        return;
    }

    flo_html_Dom *duplicatedDom = flo_html_duplicateDom(dom, &scratch);

    flo_html_ComparisonResult comp =
        flo_html_equals(dom, duplicatedDom, scratch);
    if (comp.status != COMPARISON_SUCCESS) {
        FLO_TEST_FAILURE { FLO_ERROR((FLO_STRING("Duplication failed!\n"))); }
        return;
    }

    flo_html_node_id nodeID = 0;
    flo_html_QueryStatus status = flo_html_querySelector(
        FLO_STRING("[special-one]"), dom, &nodeID, scratch);
    if (status != QUERY_SUCCESS) {
        FLO_TEST_FAILURE {
            flo_appendExpectCodeWithString(
                QUERY_SUCCESS, flo_html_queryingStatusToString(QUERY_SUCCESS),
                status, flo_html_queryingStatusToString(status));
        }
        return;
    }

    flo_html_removeBooleanProperty(nodeID, FLO_STRING("special-one"), dom);

    comp = flo_html_equals(dom, duplicatedDom, scratch);
    if (comp.status != COMPARISON_DIFFERENT_SIZES) {
        FLO_TEST_FAILURE {
            FLO_ERROR((FLO_STRING(
                "Did not recognize different content !\nReturned ")));
            FLO_ERROR(flo_html_comparisonStatusToString(comp.status),
                      FLO_NEWLINE);
        }
        return;
    }

    flo_testSuccess();
}

void testflo_html_DomDuplications(flo_Arena scratch) {
    FLO_TEST_TOPIC(FLO_STRING("DOM duplications")) {
        FLO_TEST(FLO_STRING("simple duplication")) { testDuplication(scratch); }
    }
}
