#ifndef FLO_HTML_PARSER_DOM_TRAVERSAL_H
#define FLO_HTML_PARSER_DOM_TRAVERSAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "dom.h"

/**
 * @brief Get the ID of the first child node of a given node.
 *
 * This function returns the ID of the first child node of the specified
 * `currentNodeID` within the provided `dom` structure.
 *
 * @param[in]   currentNodeID   The ID of the current node.
 * @param[in]   dom             The DOM structure.
 *
 * @return  The ID of the first child node, or 0 if there are no
 *          child nodes.
 */
flo_html_node_id flo_html_getFirstChild(flo_html_node_id currentNodeID,
                                        flo_html_Dom *dom);

/**
 * @brief Get a pointer to the first child node of a given node.
 *
 * This function returns a pointer to the `ParentChild` structure representing
 * the first child node of the specified `currentNodeID` within the provided
 * `dom` structure.
 *
 * @param[in]   currentNodeID   The ID of the current node.
 * @param[in]   dom             The DOM structure.
 *
 * @return  A pointer to the `ParentChild` structure of the first child node,
 *          or NULL if there are no child nodes.
 */
flo_html_ParentChild *flo_html_getFirstChildNode(flo_html_node_id currentNodeID,
                                                 flo_html_Dom *dom);

/**
 * @brief Get the ID of the next sibling node of a given node.
 *
 * This function returns the ID of the next sibling node of the specified
 * `currentNodeID` within the provided `dom` structure.
 *
 * @param[in]   currentNodeID   The ID of the current node.
 * @param[in]   dom             The DOM structure.
 *
 * @return  The ID of the next sibling node, or 0 if there are no
 *          more sibling nodes.
 */
flo_html_node_id flo_html_getNext(flo_html_node_id currentNodeID,
                                  flo_html_Dom *dom);

/**
 * @brief Get a pointer to the next sibling node of a given node.
 *
 * This function returns a pointer to the `NextNode` structure representing the
 * next sibling node of the specified `currentNodeID` within the provided `dom`
 * structure. `NextNode->nextNodeID` will contain the `flo_html_node_id` of the
 * next sibling node.
 *
 * @param[in]   currentNodeID   The ID of the current node.
 * @param[in]   dom             The DOM structure.
 *
 * @return  A pointer to the `NextNode` structure of the next sibling node, or
 *          NULL if there are no more sibling nodes.
 */
flo_html_NextNode *flo_html_getNextNode(flo_html_node_id currentNodeID,
                                        flo_html_Dom *dom);

/**
 * @brief Get the ID of the previous sibling node of a given node.
 *
 * This function returns the ID of the previous sibling node of the specified
 * `currentNodeID` within the provided `dom` structure.
 *
 * @param[in]   currentNodeID   The ID of the current node.
 * @param[in]   dom             The DOM structure.
 *
 * @return  The ID of the previous sibling node, or NODE_ID_NULL if there are no
 *          previous sibling nodes.
 */
flo_html_node_id flo_html_getPrevious(flo_html_node_id currentNodeID,
                                      flo_html_Dom *dom);

/**
 * @brief Get a pointer to the previous sibling node of a given node.
 *
 * This function returns a pointer to the `NextNode` structure representing the
 * previous sibling node of the specified `currentNodeID` within the provided
 * `dom` structure. `NextNode->currentNodeID` will contain the
 * `flo_html_node_id` of the previous sibling node.
 *
 * @param[in]   currentNodeID   The ID of the current node.
 * @param[in]   dom             The DOM structure.
 *
 * @return  A pointer to the `NextNode` structure of the previous sibling node,
 *          or NULL if there are no previous sibling nodes.
 */
flo_html_NextNode *flo_html_getPreviousNode(flo_html_node_id currentNodeID,
                                            flo_html_Dom *dom);

/**
 * @brief Get the ID of the parent node of a given node.
 *
 * This function returns the ID of the parent node of the specified
 * `currentNodeID` within the provided `dom` structure.
 *
 * @param[in]   currentNodeID   The ID of the current node.
 * @param[in]   dom             The DOM structure.
 *
 * @return  The ID of the parent node, or 0 if there is no parent
 * node.
 */
flo_html_node_id flo_html_getParent(flo_html_node_id currentNodeID,
                                    flo_html_Dom *dom);

/**
 * @brief Get a pointer to the parent node of a given node.
 *
 * This function returns a pointer to the `ParentChild` structure representing
 * the parent node of the specified `currentNodeID` within the provided `dom`
 * structure.
 *
 * @param[in]   currentNodeID   The ID of the current node.
 * @param[in]   dom             The DOM structure.
 *
 * @return  A pointer to the `ParentChild` structure of the parent node, or NULL
 *          if there is no parent node.
 */
flo_html_ParentChild *flo_html_getParentNode(flo_html_node_id currentNodeID,
                                             flo_html_Dom *dom);

/**
 * @brief Traverse the DOM structure to the next node.
 *
 * This function traverses the DOM structure from the specified `currentNodeID`
 * and returns the ID of the next node.
 *
 * @param[in]   currentNodeID   The ID of the current node.
 * @param[in]   dom             The DOM structure.
 *
 * @return  The ID of the next node, or 0 if there are no more nodes
 *          to traverse.
 */
flo_html_node_id flo_html_traverseDom(flo_html_node_id currentNodeID,
                                      flo_html_Dom *dom);

/**
 * @brief Traverse over a specific node within the DOM structure.
 *
 * This function traverses over the DOM structure of the specified
 * `toTraverseNodeID`. See a common use-case below.
 * flo_html_node_id currentNodeID = nodeID;
 * while ((currentNodeID = flo_html_traverseNode(currentNodeID, nodeID, dom)) !=
 * 0) {
 * }
 *
 * to the node with the given `toTraverseNodeID` and returns the ID of the
 * target node.
 *
 * @param[in]   currentNodeID       The ID of the current node.
 * @param[in]   toTraverseNodeID    The ID of the node to traverse to.
 * @param[in]   dom                 The DOM structure.
 *
 * @return  The ID of the next node inside `toTraverseNodeID`, or 0 if there are
 * not more nodes to traverse.
 */
flo_html_node_id flo_html_traverseNode(flo_html_node_id currentNodeID,
                                       flo_html_node_id toTraverseNodeID,
                                       flo_html_Dom *dom);

/**
 * @brief Get the ID of the last sibling node starting from a given node.
 *
 * This function returns the ID of the last sibling node when starting
 * from the specified `startNodeID` within the provided `dom` structure.
 *
 * @param[in]   startNodeID The ID of the starting node.
 * @param[in]   dom         The DOM structure.
 *
 * @return  The ID of the last sibling node, or 0 if there are
 *          no more sibling nodes.
 */
flo_html_node_id flo_html_getLastNext(flo_html_node_id startNodeID,
                                      flo_html_Dom *dom);

/**
 * @brief Get a pointer to the last next sibling node starting from a given
 * node.
 *
 * This function returns a pointer to the `NextNode` structure representing the
 * last next sibling node when starting from the specified `startNodeID` within
 * the provided `dom` structure. `NextNode->nextNodeID` will contain the
 * `flo_html_node_id` of the last sibling node.
 *
 * @param[in]   startNodeID The ID of the starting node.
 * @param[in]   dom         The DOM structure.
 *
 * @return  A pointer to the `NextNode` structure of the last sibling node,
 *          or NULL if there are no more sibling nodes.
 */
flo_html_NextNode *flo_html_getLastNextNode(flo_html_node_id startNodeID,
                                            flo_html_Dom *dom);

#ifdef __cplusplus
}
#endif

#endif
