#ifndef FLO_HTML_PARSER_DOM_TRAVERSAL_H
#define FLO_HTML_PARSER_DOM_TRAVERSAL_H

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
node_id getFirstChild(node_id currentNodeID, const Dom *dom);

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
ParentChild *getFirstChildNode(node_id currentNodeID, const Dom *dom);

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
node_id getNext(node_id currentNodeID, const Dom *dom);

/**
 * @brief Get a pointer to the next sibling node of a given node.
 *
 * This function returns a pointer to the `NextNode` structure representing the
 * next sibling node of the specified `currentNodeID` within the provided `dom`
 * structure. `NextNode->nextNodeID` will contain the `node_id` of the next
 * sibling node.
 *
 * @param[in]   currentNodeID   The ID of the current node.
 * @param[in]   dom             The DOM structure.
 *
 * @return  A pointer to the `NextNode` structure of the next sibling node, or
 *          NULL if there are no more sibling nodes.
 */
NextNode *getNextNode(node_id currentNodeID, const Dom *dom);

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
node_id getPrevious(node_id currentNodeID, const Dom *dom);

/**
 * @brief Get a pointer to the previous sibling node of a given node.
 *
 * This function returns a pointer to the `NextNode` structure representing the
 * previous sibling node of the specified `currentNodeID` within the provided
 * `dom` structure. `NextNode->currentNodeID` will contain the `node_id` of the
 * previous sibling node.
 *
 * @param[in]   currentNodeID   The ID of the current node.
 * @param[in]   dom             The DOM structure.
 *
 * @return  A pointer to the `NextNode` structure of the previous sibling node,
 *          or NULL if there are no previous sibling nodes.
 */
NextNode *getPreviousNode(node_id currentNodeID, const Dom *dom);

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
node_id getParent(node_id currentNodeID, const Dom *dom);

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
ParentChild *getParentNode(node_id currentNodeID, const Dom *dom);

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
node_id traverseDom(node_id currentNodeID, const Dom *dom);

/**
 * @brief Traverse over a specific node within the DOM structure.
 *
 * This function traverses over the DOM structure of the specified
 * `toTraverseNodeID`. See a common use-case below.
 * node_id currentNodeID = nodeID;
 * while ((currentNodeID = traverseNode(currentNodeID, nodeID, dom)) != 0) {
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
node_id traverseNode(node_id currentNodeID, node_id toTraverseNodeID,
                     const Dom *dom);

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
node_id getLastNext(node_id startNodeID, const Dom *dom);

/**
 * @brief Get a pointer to the last next sibling node starting from a given
 * node.
 *
 * This function returns a pointer to the `NextNode` structure representing the
 * last next sibling node when starting from the specified `startNodeID` within
 * the provided `dom` structure. `NextNode->nextNodeID` will contain the
 * `node_id` of the last sibling node.
 *
 * @param[in]   startNodeID The ID of the starting node.
 * @param[in]   dom         The DOM structure.
 *
 * @return  A pointer to the `NextNode` structure of the last sibling node,
 *          or NULL if there are no more sibling nodes.
 */
NextNode *getLastNextNode(node_id startNodeID, const Dom *dom);

#endif
