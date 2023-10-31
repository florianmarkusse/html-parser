#ifndef FLO_HTML_PARSER_DOM_READING_READING_H
#define FLO_HTML_PARSER_DOM_READING_READING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "flo/html-parser/definitions.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/query/query-status.h"
#include "flo/html-parser/node/node.h"

/**
 * @brief Check if a DOM node has a boolean property.
 *
 * This function checks if a DOM node identified by `nodeID` has the specified
 * boolean property `boolProp` within the given DOM `dom`.
 *
 * @param[in]   nodeID          The ID of the node to query.
 * @param[in]   boolProp        The name of the boolean property to check.
 * @param[in]   dom             The DOM structure.
 *
 * @return  True if the property exists and is true, otherwise false.
 */
bool flo_html_hasBoolProp(flo_html_node_id nodeID, flo_html_String boolProp,
                          flo_html_Dom *dom);

/**
 * @brief Check if a DOM node has a property with a given key.
 *
 * This function checks if a DOM node identified by `nodeID` has a property with
 * the specified key `propKey` within the given DOM `dom`.
 *
 * @param[in]   nodeID          The ID of the node to query.
 * @param[in]   propKey         The key of the property to check.
 * @param[in]   dom             The DOM structure.
 *
 * @return  True if a property with the key exists, otherwise false.
 */
bool flo_html_hasPropKey(flo_html_node_id nodeID, flo_html_String propKey,
                         flo_html_Dom *dom);

/**
 * @brief Check if a DOM node has a property with a given value.
 *
 * This function checks if a DOM node identified by `nodeID` has a property with
 * the specified value `propValue` within the given DOM `dom`.
 *
 * @param[in]   nodeID          The ID of the node to query.
 * @param[in]   propValue       The value of the property to check.
 * @param[in]   dom             The DOM structure.
 *
 * @return  True if a property with the value exists, otherwise false.
 */
bool flo_html_hasPropValue(flo_html_node_id nodeID, flo_html_String propValue,
                           flo_html_Dom *dom);

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
 *
 * @return  True if a property with the specified key and value exists,
 *          otherwise false.
 */
bool flo_html_hasProperty(flo_html_node_id nodeID, flo_html_String propKey,
                          flo_html_String propValue, flo_html_Dom *dom);

/**
 * @brief Get the value of a property associated with a DOM node.
 *
 * This function retrieves the value of a property with the specified `propKey`
 * associated with a DOM node identified by `nodeID` within the given DOM `dom`.
 *
 * @param[in]   nodeID          The ID of the node to query.
 * @param[in]   propKey         The key of the property to retrieve.
 * @param[in]   dom             The DOM structure.
 *
 * @return  The value of the property if it exists, or NULL if not found.
 */
flo_html_String flo_html_getValue(flo_html_node_id nodeID,
                                  flo_html_String propKey, flo_html_Dom *dom);

/**
 * @brief Get the text content of a DOM node.
 *
 * This function retrieves the text content of a DOM node identified by `nodeID`
 * within the given DOM `dom`. The results are stored in an array of strings.
 *
 * @param[in]   nodeID          The ID of the node to query.
 * @param[in]   dom             The DOM structure.
 * @param[in]   perm            The permanent memory arena.
 *
 * @return  An array containing all the text element children of the node ID.
 *
 * @note    The caller is responsible for freeing the memory allocated for
 *          'results' when no longer needed.
 */
flo_html_String_d_a flo_html_getTextContent(flo_html_node_id nodeID,
                                            flo_html_Dom *dom,
                                            flo_html_Arena *perm);

#ifdef __cplusplus
}
#endif

#endif
