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
void removeNode(node_id nodeID, Dom *dom);

/**
 * @brief Remove all children of a node in the DOM.
 *
 * This function removes all child nodes of the node identified by `nodeID`
 * within the DOM structure.
 *
 * @param[in]   nodeID  The ID of the node whose children will be removed.
 * @param[in]   dom     The DOM structure.
 */
void removeChildren(node_id nodeID, Dom *dom);

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
void removeBooleanProperty(node_id nodeID, const char *boolProp, Dom *dom,
                           const TextStore *textStore);

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
void removeProperty(node_id nodeID, const char *keyProp, Dom *dom,
                    const TextStore *textStore);

#endif
