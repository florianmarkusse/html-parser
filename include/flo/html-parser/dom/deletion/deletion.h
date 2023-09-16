#ifndef FLO_HTML_PARSER_DOM_DELETION_DELETION_H
#define FLO_HTML_PARSER_DOM_DELETION_DELETION_H

#include "flo/html-parser/dom/dom.h"

/**
 * @brief Remove a node from the DOM.
 *
 * This function removes the node identified by `nodeID` from the DOM structure.
 * Any child nodes are also removed.
 *
 * @param[in]   nodeID  The ID of the node to remove.
 * @param[in]   dom     The DOM structure.
 */
void flo_html_removeNode(flo_html_node_id nodeID, flo_html_Dom *dom);

/**
 * @brief Remove all children of a node in the DOM.
 *
 * This function removes all child nodes of the node identified by `nodeID`
 * within the DOM structure.
 *
 * @param[in]   nodeID  The ID of the node whose children will be removed.
 * @param[in]   dom     The DOM structure.
 */
void flo_html_removeChildren(flo_html_node_id nodeID, flo_html_Dom *dom);

/**
 * @brief Remove a boolean property from a node in the DOM.
 *
 * This function removes the specified boolean property `boolProp` from the node
 * identified by `nodeID` within the DOM structure.
 *
 * @param[in]   nodeID          The ID of the node from which to remove the
 *                              property.
 * @param[in]   boolProp        The name of the boolean property to remove.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore   The text store.
 */
void flo_html_removeBooleanProperty(flo_html_node_id nodeID, const char *boolProp,
                                    flo_html_Dom *dom,
                                    const flo_html_TextStore *textStore);

/**
 * @brief Remove a property with a given key from a node in the DOM.
 *
 * This function removes the property with the specified key `keyProp` from the
 * node identified by `nodeID` within the DOM structure.
 *
 * @param[in]   nodeID          The ID of the node from which to remove the
 *                              property.
 * @param[in]   keyProp         The key of the property to remove.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore   The text store.
 */
void flo_html_removeProperty(flo_html_node_id nodeID, const char *keyProp,
                             flo_html_Dom *dom,
                             const flo_html_TextStore *textStore);

#endif
