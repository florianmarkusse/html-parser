#ifndef FLO_HTML_PARSER_DOM_QUERY_QUERY_H
#define FLO_HTML_PARSER_DOM_QUERY_QUERY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "flo/html-parser/definitions.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/util/array.h"
#include "query-status.h"

typedef FLO_ARRAY(uint16_t) flo_html_node_id_a;

/**
 * @brief Query for elements matching a CSS selector in the DOM.
 *
 * This function queries the DOM for elements matching the specified CSS query
 * `cssQuery`. It returns an array of node IDs representing the matching
 * elements in the `results` parameter, and the number of matching elements in
 * `resultsLen`.
 *
 * @param[in]   cssQuery        The CSS selector query to match elements.
 * @param[in]   dom             The DOM structure to query.
 * @param[out]  results         An array of node IDs representing matching
 *                              elements.
 * @param[in]   perm            The memory arena for permanent allocations.
 *
 * @return  The query status (QUERY_SUCCESS if completed, different
 *          otherwise). See @ref
 *          "flo/html-parser/dom/query/query-status.h#flo_html_QueryStatus".
 *
 * @note    The caller is responsible for freeing the memory allocated for
 *          'results' when no longer needed.
 */
flo_html_QueryStatus flo_html_querySelectorAll(flo_String cssQuery,
                                               flo_html_Dom *dom,
                                               flo_html_node_id_a *results,
                                               flo_Arena *perm);

/**
 * @brief Query for the first element matching a CSS selector in the DOM.
 *
 * This function queries the DOM for the first element matching the specified
 * CSS query `cssQuery`. It returns the node ID of the first matching element in
 * the `result` parameter.
 *
 * @param[in]   cssQuery        The CSS selector query to match the element.
 * @param[in]   dom             The DOM structure to query.
 * @param[out]  result          The node ID of the first matching element.
 * @param[in]   scratch         The memory arena for temporary allocations.
 *
 * @return  The query status (QUERY_SUCCESS if completed, different
 *          otherwise). See @ref
 *          "flo/html-parser/dom/query/query-status.h#flo_html_QueryStatus".
 */
flo_html_QueryStatus flo_html_querySelector(flo_String cssQuery,
                                            flo_html_Dom *dom,
                                            flo_html_node_id *result,
                                            flo_Arena scratch);

#ifdef __cplusplus
}
#endif

#endif
