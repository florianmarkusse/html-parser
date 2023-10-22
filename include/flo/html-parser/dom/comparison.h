#ifndef FLO_HTML_PARSER_DOM_COMPARISON_H
#define FLO_HTML_PARSER_DOM_COMPARISON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "flo/html-parser/comparison-status.h"
#include "flo/html-parser/dom/dom.h"

typedef struct {
    flo_html_ComparisonStatus status;
    flo_html_node_id nodeID1;
    flo_html_node_id nodeID2;
} flo_html_ComparisonResult;

/**
 * @brief Compare two DOM structures.
 *
 * This function compares two DOM structures specified by `dom1` and `dom2`. It
 * also requires the respective `TextStore` instances for each DOM. The
 * comparison checks if the two DOM structures are equal.
 *
 * @param[in]   dom1                The first DOM structure to compare.
 * @param[in]   textStore1      The text store for the first DOM.
 * @param[in]   dom2                The second DOM structure to compare.
 * @param[in]   textStore2      The text store for the second DOM.
 *
 * @return  The comparison status (COMPARISON_SUCCESS if equal, different
 *          otherwise). See @ref
 *          "flo/html-parser/comparison-status.h#flo_html_ComparisonStatus".
 */
flo_html_ComparisonResult
flo_html_equals(flo_html_Dom *dom1, flo_html_Dom *dom2, flo_html_Arena scratch);

/**
 * @brief Print the first difference between two nodes.
 *
 * This function compares two nodes specified by `nodeID1` and `nodeID2`. The
 * function prints the first difference found in the two two nodes.
 *
 * @param[in]   nodeID1             The node ID in the first DOM.
 * @param[in]   dom1                The first DOM structure to compare.
 * @param[in]   textStore1      The text store for the first DOM.
 * @param[in]   nodeID2             The node ID in the second DOM.
 * @param[in]   dom2                The second DOM structure to compare.
 * @param[in]   textStore2      The text store for the second DOM.
 */
void flo_html_printFirstDifference(flo_html_node_id nodeID1, flo_html_Dom *dom1,
                                   flo_html_node_id nodeID2, flo_html_Dom *dom2,
                                   flo_html_Arena scratch);

#ifdef __cplusplus
}
#endif

#endif
