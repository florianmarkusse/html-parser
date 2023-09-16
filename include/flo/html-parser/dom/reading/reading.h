#ifndef FLO_HTML_PARSER_DOM_READING_READING_H
#define FLO_HTML_PARSER_DOM_READING_READING_H

#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/query/query-status.h"
#include "flo/html-parser/type/node/node.h"

/**
 * @brief Get the type of a DOM node.
 *
 * This function retrieves the type of a DOM node identified by `nodeID` within
 * the given DOM `dom`.
 *
 * @param[in]   nodeID          The ID of the node to query.
 * @param[in]   dom             The DOM structure.
 *
 * @return  The type of the node (e.g., NODE_TYPE_DOCUMENT, NODE_TYPE_TEXT,
 * etc.). See @ref /flo/html-parser/type/node/node.h#flo_html_NodeType.
 */
flo_html_NodeType flo_html_getflo_html_NodeType(flo_html_node_id nodeID,
                                                const flo_html_Dom *dom);

/**
 * @brief Check if a DOM node has a boolean property.
 *
 * This function checks if a DOM node identified by `nodeID` has the specified
 * boolean property `boolProp` within the given DOM `dom`.
 *
 * @param[in]   nodeID          The ID of the node to query.
 * @param[in]   boolProp        The name of the boolean property to check.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore   The text store.
 *
 * @return  True if the property exists and is true, otherwise false.
 */
bool flo_html_hasBoolProp(flo_html_node_id nodeID, const char *boolProp,
                          const flo_html_Dom *dom,
                          const flo_html_TextStore *textStore);

/**
 * @brief Check if a DOM node has a property with a given key.
 *
 * This function checks if a DOM node identified by `nodeID` has a property with
 * the specified key `propKey` within the given DOM `dom`.
 *
 * @param[in]   nodeID          The ID of the node to query.
 * @param[in]   propKey         The key of the property to check.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore   The text store.
 *
 * @return  True if a property with the key exists, otherwise false.
 */
bool flo_html_hasPropKey(flo_html_node_id nodeID, const char *propKey,
                         const flo_html_Dom *dom,
                         const flo_html_TextStore *textStore);

/**
 * @brief Check if a DOM node has a property with a given value.
 *
 * This function checks if a DOM node identified by `nodeID` has a property with
 * the specified value `propValue` within the given DOM `dom`.
 *
 * @param[in]   nodeID          The ID of the node to query.
 * @param[in]   propValue       The value of the property to check.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore   The text store.
 *
 * @return  True if a property with the value exists, otherwise false.
 */
bool flo_html_hasPropValue(flo_html_node_id nodeID, const char *propValue,
                           const flo_html_Dom *dom,
                           const flo_html_TextStore *textStore);

/**
 * @brief Check if a DOM node has a property with a specific key and value.
 *
 * This function checks if a DOM node identified by `nodeID` has a property with
 * the specified `propKey` and `propValue` within the given DOM `dom`.
 *
 * @param[in]   nodeID          The ID of the node to query.
 * @param[in]   propKey         The key of the property to check.
 * @param[in]   propValue       The value of the property to check.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore   The text store.
 *
 * @return  True if a property with the specified key and value exists,
 *          otherwise false.
 */
bool flo_html_hasProperty(flo_html_node_id nodeID, const char *propKey,
                          const char *propValue, const flo_html_Dom *dom,
                          const flo_html_TextStore *textStore);

/**
 * @brief Get the value of a property associated with a DOM node.
 *
 * This function retrieves the value of a property with the specified `propKey`
 * associated with a DOM node identified by `nodeID` within the given DOM `dom`.
 *
 * @param[in]   nodeID          The ID of the node to query.
 * @param[in]   propKey         The key of the property to retrieve.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore   The text store.
 *
 * @return  The value of the property if it exists, or NULL if not found.
 */
const char *flo_html_getValue(flo_html_node_id nodeID, const char *propKey,
                              const flo_html_Dom *dom,
                              const flo_html_TextStore *textStore);

/**
 * @brief Get the text content of a DOM node.
 *
 * This function retrieves the text content of a DOM node identified by `nodeID`
 * within the given DOM `dom`. The results are stored in an array of strings.
 *
 * @param[in]   nodeID          The ID of the node to query.
 * @param[in]   dom             The DOM structure.
 * @param[out]  results         A pointer to an array of strings to store
 *                              results.
 * @param[out]  resultsLen      The length of the results array.
 *
 * @return  The query status (QUERY_SUCCESS if successful, an error code
 * otherwise).
 *
 * @note    The caller is responsible for freeing the memory allocated for
 *          'results' when no longer needed.
 */
flo_html_QueryStatus flo_html_getTextContent(flo_html_node_id nodeID,
                                             const flo_html_Dom *dom,
                                             const char ***results,
                                             size_t *resultsLen);

#endif
