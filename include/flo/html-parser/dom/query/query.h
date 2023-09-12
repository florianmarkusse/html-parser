#ifndef FLO_HTML_PARSER_DOM_QUERY_QUERY_H
#define FLO_HTML_PARSER_DOM_QUERY_QUERY_H

#include "flo/html-parser/dom/dom.h"
#include "query-status.h"

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
 * @param[in]   textStore   The text store for the DOM.
 * @param[out]  results         An array of node IDs representing matching
 *                              elements.
 * @param[out]  resultsLen      The number of matching elements found.
 *
 * @return  The query status (QUERY_SUCCESS if completed, different
 *          otherwise). See @ref
 *          "flo/html-parser/dom/query/query-status.h#QueryStatus".
 *
 * @note    The caller is responsible for freeing the memory allocated for
 *          'results' when no longer needed.
 */
QueryStatus querySelectorAll(const char *cssQuery, const Dom *dom,
                             const TextStore *textStore,
                             node_id **results, size_t *resultsLen);

/**
 * @brief Get elements by their class name in the DOM.
 *
 * This function retrieves elements in the DOM that have the specified CSS class
 * name `class`. It returns an array of node IDs representing the matching
 * elements in the `results` parameter, and the number of matching elements in
 * `resultsLen`.
 *
 * @param[in]   class           The CSS class name to match elements.
 * @param[in]   dom             The DOM structure to query.
 * @param[in]   textStore   The text store for the DOM.
 * @param[out]  results         An array of node IDs representing matching
 *                              elements.
 * @param[out]  resultsLen      The number of matching elements found.
 *
 * @return  The query status (QUERY_SUCCESS if completed, different
 *          otherwise). See @ref
 *          "flo/html-parser/dom/query/query-status.h#QueryStatus".
 *
 * @note    The caller is responsible for freeing the memory allocated for
 *          'results' when no longer needed.
 */
QueryStatus getElementsByClassName(const char *class, const Dom *dom,
                                   const TextStore *textStore,
                                   node_id **results, size_t *resultsLen);

/**
 * @brief Get elements by their tag name in the DOM.
 *
 * This function retrieves elements in the DOM that have the specified HTML tag
 * name `tag`. It returns an array of node IDs representing the matching
 * elements in the `results` parameter, and the number of matching elements in
 * `resultsLen`.
 *
 * @param[in]   tag             The HTML tag name to match elements.
 * @param[in]   dom             The DOM structure to query.
 * @param[in]   textStore   The text store for the DOM.
 * @param[out]  results         An array of node IDs representing matching
 *                              elements.
 * @param[out]  resultsLen      The number of matching elements found.
 *
 * @return  The query status (QUERY_SUCCESS if completed, different
 *          otherwise). See @ref
 *          "flo/html-parser/dom/query/query-status.h#QueryStatus".
 *
 * @note    The caller is responsible for freeing the memory allocated for
 *          'results' when no longer needed.
 */
QueryStatus getElementsByTagName(const char *tag, const Dom *dom,
                                 const TextStore *textStore,
                                 node_id **results, size_t *resultsLen);

/**
 * @brief Query for the first element matching a CSS selector in the DOM.
 *
 * This function queries the DOM for the first element matching the specified
 * CSS query `cssQuery`. It returns the node ID of the first matching element in
 * the `result` parameter.
 *
 * @param[in]   cssQuery        The CSS selector query to match the element.
 * @param[in]   dom             The DOM structure to query.
 * @param[in]   textStore   The text store for the DOM.
 * @param[out]  result          The node ID of the first matching element.
 *
 * @return  The query status (QUERY_SUCCESS if completed, different
 *          otherwise). See @ref
 *          "flo/html-parser/dom/query/query-status.h#QueryStatus".
 */
QueryStatus querySelector(const char *cssQuery, const Dom *dom,
                          const TextStore *textStore, node_id *result);

/**
 * @brief Get an element by its ID attribute in the DOM.
 *
 * This function retrieves an element in the DOM with the specified HTML `id`
 * attribute. It returns the node ID of the matching element in the `result`
 * parameter.
 *
 * @param[in]   id              The HTML `id` attribute value to match.
 * @param[in]   dom             The DOM structure to query.
 * @param[in]   textStore   The text store for the DOM.
 * @param[out]  result          The node ID of the matching element.
 *
 * @return  The query status (QUERY_SUCCESS if completed, different
 *          otherwise). See @ref
 *          "flo/html-parser/dom/query/query-status.h#QueryStatus".
 */
QueryStatus getElementByID(const char *id, const Dom *dom,
                           const TextStore *textStore, node_id *result);

#endif
