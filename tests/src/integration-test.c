#include <flo/html-parser.h>
#include <memory/arena.h>

#include "comparison-test.h"
#include "expectations.h"
#include "integration-test.h"
#include "flo/util/log.h"
#include "test.h"

#define CURRENT_DIR "tests/src/inputs/"
#define PARSE_QUERY_MODIFY_BEFORE CURRENT_DIR "parse-query-modify-before.html"
#define PARSE_QUERY_MODIFY_AFTER CURRENT_DIR "parse-query-modify-after.html"

static void parseQueryModify(flo_Arena scratch) {
    FLO_TEST(FLO_STRING("Parse/Query/Modify")) {
        ComparisonTest comparisonTest = initComparisonTest(
            PARSE_QUERY_MODIFY_BEFORE, PARSE_QUERY_MODIFY_AFTER, &scratch);

        flo_html_node_id_a results;
        flo_html_QueryStatus actual = flo_html_querySelectorAll(
            FLO_STRING("title"), comparisonTest.actual, &results, &scratch);

        if (actual != QUERY_SUCCESS) {
            FLO_TEST_FAILURE {
                (flo_appendExpectCodeWithString(
                    QUERY_SUCCESS,
                    flo_html_queryingStatusToString(QUERY_SUCCESS), actual,
                    flo_html_queryingStatusToString(actual)));
            }
            return;
        }

        if (results.len != 1) {
            FLO_TEST_FAILURE {
                FLO_ERROR((FLO_STRING("Node IDs received...\n")));
                for (ptrdiff_t i = 0; i < results.len; i++) {
                    FLO_ERROR(results.buf[i], FLO_NEWLINE);
                }
            }
            return;
        }
        flo_html_setTextContent(results.buf[0], FLO_STRING("FOURTH"),
                                comparisonTest.actual, &scratch);
        flo_html_addBooleanPropertyToNode(results.buf[0],
                                          FLO_STRING("the-fourth"),
                                          comparisonTest.actual, &scratch);
        flo_html_addPropertyToNode(results.buf[0], FLO_STRING("the-property"),
                                   FLO_STRING("my value"),
                                   comparisonTest.actual, &scratch);

        flo_html_node_id currentNodeID = 0;
        actual = flo_html_querySelector(
            FLO_STRING("head"), comparisonTest.actual, &currentNodeID, scratch);
        if (actual != QUERY_SUCCESS) {
            FLO_TEST_FAILURE {
                flo_appendExpectCodeWithString(
                    QUERY_SUCCESS,
                    flo_html_queryingStatusToString(QUERY_SUCCESS), actual,
                    flo_html_queryingStatusToString(actual));
            }
            return;
        }

        if (flo_html_prependHTMLFromString(
                currentNodeID,
                FLO_STRING("<title "
                           "id=\"first-title-tag\"></title><title>FIRST</"
                           "title><title>SECOND</title><title>THIRD</title>"),
                comparisonTest.actual, &scratch) == 0) {
            FLO_TEST_FAILURE {
                FLO_ERROR(
                    (FLO_STRING("Failed to prepend HTML from string.\n")));
            }
            return;
        }

        actual = flo_html_querySelectorAll(
            FLO_STRING("title"), comparisonTest.actual, &results, &scratch);
        if (actual != QUERY_SUCCESS) {
            FLO_TEST_FAILURE {
                flo_appendExpectCodeWithString(
                    QUERY_SUCCESS,
                    flo_html_queryingStatusToString(QUERY_SUCCESS), actual,
                    flo_html_queryingStatusToString(actual));
            }
            return;
        }

        if (results.len != 5) {
            FLO_TEST_FAILURE {
                flo_appendExpectUint(5, results.len);
                FLO_ERROR((FLO_STRING("Node IDs received...\n")));
                for (ptrdiff_t i = 0; i < results.len; i++) {
                    FLO_ERROR(results.buf[i], FLO_NEWLINE);
                }
            }
            return;
        }

        actual = flo_html_querySelector(FLO_STRING("#first-title-tag"),
                                        comparisonTest.actual, &currentNodeID,
                                        scratch);
        if (actual != QUERY_SUCCESS) {
            FLO_TEST_FAILURE {
                flo_appendExpectCodeWithString(
                    QUERY_SUCCESS,
                    flo_html_queryingStatusToString(QUERY_SUCCESS), actual,
                    flo_html_queryingStatusToString(actual));
            }
            return;
        }

        flo_html_removeNode(currentNodeID, comparisonTest.actual);

        actual = flo_html_querySelectorAll(
            FLO_STRING("title"), comparisonTest.actual, &results, &scratch);
        if (actual != QUERY_SUCCESS) {
            FLO_TEST_FAILURE {
                flo_appendExpectCodeWithString(
                    QUERY_SUCCESS,
                    flo_html_queryingStatusToString(QUERY_SUCCESS), actual,
                    flo_html_queryingStatusToString(actual));
            }
            return;
        }

        if (results.len != 4) {
            FLO_TEST_FAILURE {
                FLO_ERROR((FLO_STRING("Node IDs received...\n")));
                for (ptrdiff_t i = 0; i < results.len; i++) {
                    FLO_ERROR(results.buf[i], FLO_NEWLINE);
                }
            }
            return;
        }

        flo_html_Dom *duplicatedDom =
            flo_html_duplicateDom(comparisonTest.actual, &scratch);

        flo_html_ComparisonResult comp =
            flo_html_equals(comparisonTest.actual, duplicatedDom, scratch);
        if (comp.status != COMPARISON_SUCCESS) {
            FLO_TEST_FAILURE {
                FLO_ERROR((FLO_STRING("Duplication failed!\n")));
            }
            return;
        }

        compareAndEndTest(&comparisonTest, scratch);
        flo_html_printHTML(comparisonTest.actual);
    }
}

void testIntegrations(flo_Arena scratch) {
    FLO_TEST_TOPIC(FLO_STRING("Integration tests")) {
        parseQueryModify(scratch);
    }
}
