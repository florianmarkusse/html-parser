
#include <string.h>

#include "flo/html-parser/dom/deletion/deletion.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/modification/modification.h"
#include "flo/html-parser/dom/query/query-status.h"
#include "flo/html-parser/dom/query/query.h"
#include "flo/html-parser/dom/registry.h"
#include "flo/html-parser/dom/replacement/replacement.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/dom/utils.h"
#include "flo/html-parser/dom/writing.h"
#include "flo/html-parser/parser/parser.h"
#include "flo/html-parser/type/node/node.h"
#include "flo/html-parser/type/node/parent-child.h"
#include "flo/html-parser/utils/file/read.h"
#include "flo/html-parser/utils/print/error.h"

#define REPLACE_USING_QUERYSELECTOR(cssQuery, nodeData, dom, dataContainer,    \
                                    replaceWithFunction)                       \
    do {                                                                       \
        node_id parentNodeID = 0;                                              \
        QueryStatus queryResult =                                              \
            querySelector(cssQuery, dom, dataContainer, &parentNodeID);        \
        if (queryResult != QUERY_SUCCESS) {                                    \
            PRINT_ERROR("Could not find element using query selector: %s\n",   \
                        cssQuery);                                             \
            return DOM_NO_ELEMENT;                                             \
        }                                                                      \
        return replaceWithFunction(parentNodeID, nodeData, dom,                \
                                   dataContainer);                             \
    } while (0)

DomStatus replaceWithDocumentNodeWithQuery(const char *cssQuery,
                                           const DocumentNode *docNode,
                                           Dom *dom,
                                           DataContainer *dataContainer) {
    REPLACE_USING_QUERYSELECTOR(cssQuery, docNode, dom, dataContainer,
                                replaceWithDocumentNode);
}

DomStatus replaceWithTextNodeWithQuery(const char *cssQuery, const char *text,
                                       Dom *dom, DataContainer *dataContainer) {
    REPLACE_USING_QUERYSELECTOR(cssQuery, text, dom, dataContainer,
                                replaceWithTextNode);
}

DomStatus replaceWithNodeFromStringWithQuery(const char *cssQuery,
                                             const char *htmlString, Dom *dom,
                                             DataContainer *dataContainer) {
    REPLACE_USING_QUERYSELECTOR(cssQuery, htmlString, dom, dataContainer,
                                replaceWithNodesFromString);
}

DomStatus replaceWithNodeFromFileWithQuery(const char *cssQuery,
                                           const char *fileLocation, Dom *dom,
                                           DataContainer *dataContainer) {
    char *buffer = NULL;
    FileStatus fileStatus = readFile(fileLocation, &buffer);
    if (fileStatus != FILE_SUCCESS) {
        ERROR_WITH_CODE_FORMAT(fileStatusToString(fileStatus),
                               "Failed to read file: \"%s\"", fileLocation);
        return DOM_ERROR_MEMORY;
    }

    REPLACE_USING_QUERYSELECTOR(cssQuery, buffer, dom, dataContainer,
                                replaceWithNodesFromString);
}

static DomStatus updateReferences(const node_id toReplaceNodeID,
                                  const node_id newNodeID, Dom *dom) {
    Node *nodeToReplace = &dom->nodes[toReplaceNodeID];
    const node_id lastNextOfNew = getLastNext(newNodeID, dom);

    if (dom->firstNodeID == toReplaceNodeID) {
        NextNode *nextNode = getNextNode(toReplaceNodeID, dom);
        if (nextNode != NULL) {
            nextNode->currentNodeID = lastNextOfNew;
        }

        dom->firstNodeID = newNodeID;
        nodeToReplace->nodeType = NODE_TYPE_REMOVED;
        return DOM_SUCCESS;
    }

    ParentChild *parentChildNode = getParentNode(toReplaceNodeID, dom);
    ParentChild *parentFirstChildNode = NULL;
    if (parentChildNode != NULL) {
        parentFirstChildNode =
            getFirstChildNode(parentChildNode->parentID, dom);
    }

    NextNode *previousNext = getPreviousNode(toReplaceNodeID, dom);
    NextNode *nextNode = getNextNode(toReplaceNodeID, dom);

    // If there is no previous next node, it must mean we either are the first
    // child or are a root element
    if (previousNext != NULL) {
        previousNext->nextNodeID = newNodeID;
    } else {
        if (parentFirstChildNode != NULL) {
            parentFirstChildNode->childID = newNodeID;
        }
    }

    if (parentChildNode != NULL) {
        parentChildNode->childID = newNodeID;
        node_id otherNewNodeID = getNext(newNodeID, dom);
        while (otherNewNodeID > 0) {
            DomStatus domStatus = addParentChild(parentFirstChildNode->parentID,
                                                 otherNewNodeID, dom);
            if (domStatus != DOM_SUCCESS) {
                PRINT_ERROR("Failed to add new node ID as child!\n");
                return domStatus;
            }
            otherNewNodeID = getNext(otherNewNodeID, dom);
        }
    }

    if (nextNode != NULL) {
        nextNode->currentNodeID = lastNextOfNew;
    }

    nodeToReplace->nodeType = NODE_TYPE_REMOVED;

    return DOM_SUCCESS;
}

DomStatus replaceWithDocumentNode(node_id toReplaceNodeID,
                                  const DocumentNode *docNode, Dom *dom,
                                  DataContainer *dataContainer) {
    node_id newNodeID = 0;
    DomStatus domStatus =
        parseDocumentElement(docNode, dom, dataContainer, &newNodeID);
    if (domStatus != DOM_SUCCESS) {
        PRINT_ERROR("Failed to parse document element!\n");
        return domStatus;
    }
    return updateReferences(toReplaceNodeID, newNodeID, dom);
}

MergeResult tryMergeBothSides(const node_id toReplaceNodeID,
                              const node_id replacingNodeID, Dom *dom,
                              DataContainer *dataContainer) {
    Node *replacingNode = &dom->nodes[replacingNodeID];
    if (replacingNode->nodeType == NODE_TYPE_TEXT) {
        NextNode *previousNode = getPreviousNode(toReplaceNodeID, dom);
        if (previousNode != NULL) {
            MergeResult mergeTry =
                tryMerge(&dom->nodes[previousNode->currentNodeID],
                         replacingNode, dom, dataContainer, true);
            if (mergeTry == FAILED_MERGE || mergeTry == COMPLETED_MERGE) {
                return mergeTry;
            }
        }

        NextNode *nextNode = getNextNode(toReplaceNodeID, dom);
        if (nextNode != NULL) {
            MergeResult mergeTry =
                tryMerge(&dom->nodes[nextNode->nextNodeID], replacingNode, dom,
                         dataContainer, false);
            return mergeTry;
        }
    }

    return NO_MERGE;
}

DomStatus replaceWithTextNode(node_id toReplaceNodeID, const char *text,
                              Dom *dom, DataContainer *dataContainer) {
    node_id newNodeID = 0;
    DomStatus domStatus =
        parseTextElement(text, dom, dataContainer, &newNodeID);
    if (domStatus != DOM_SUCCESS) {
        PRINT_ERROR("Failed to parse text element!\n");
        return domStatus;
    }

    MergeResult mergeResult =
        tryMergeBothSides(toReplaceNodeID, newNodeID, dom, dataContainer);
    if (mergeResult == COMPLETED_MERGE) {
        removeNode(newNodeID, dom);
        removeNode(toReplaceNodeID, dom);
        return domStatus;
    }

    if (mergeResult == FAILED_MERGE) {
        return DOM_NO_ADD;
    }

    return updateReferences(toReplaceNodeID, newNodeID, dom);
}

DomStatus replaceWithNodesFromString(node_id toReplaceNodeID,
                                     const char *htmlString, Dom *dom,
                                     DataContainer *dataContainer) {
    node_id firstNewAddedNode = dom->nodeLen;
    DomStatus domStatus = parse(htmlString, dom, dataContainer);
    if (domStatus != DOM_SUCCESS) {
        PRINT_ERROR("Failed to parse string!\n");
        return domStatus;
    }

    node_id lastNextNode = getLastNext(firstNewAddedNode, dom);
    if (lastNextNode > firstNewAddedNode) {
        Node *firstAddedNode = &dom->nodes[firstNewAddedNode];
        if (firstAddedNode->nodeType == NODE_TYPE_TEXT) {
            NextNode *previousNode = getPreviousNode(toReplaceNodeID, dom);
            if (previousNode != NULL) {
                MergeResult mergeResult =
                    tryMerge(&dom->nodes[previousNode->currentNodeID],
                             firstAddedNode, dom, dataContainer, true);
                if (mergeResult == COMPLETED_MERGE) {
                    size_t secondNewAddedNode = getNext(firstNewAddedNode, dom);
                    removeNode(firstNewAddedNode, dom);
                    firstNewAddedNode = secondNewAddedNode;
                }

                if (mergeResult == FAILED_MERGE) {
                    return DOM_NO_ADD;
                }
            }
        }

        Node *lastAddedNode = &dom->nodes[lastNextNode];
        if (lastAddedNode->nodeType == NODE_TYPE_TEXT) {
            NextNode *nextNode = getNextNode(toReplaceNodeID, dom);
            if (nextNode != NULL) {
                MergeResult mergeResult = tryMerge(
                    &dom->nodes[nextNode->nextNodeID],
                    &dom->nodes[lastNextNode], dom, dataContainer, false);

                if (mergeResult == COMPLETED_MERGE) {
                    removeNode(lastNextNode, dom);
                }

                if (mergeResult == FAILED_MERGE) {
                    return DOM_NO_ADD;
                }
            }
        }
    } else {
        MergeResult mergeResult = tryMergeBothSides(
            toReplaceNodeID, firstNewAddedNode, dom, dataContainer);
        if (mergeResult == COMPLETED_MERGE) {
            removeNode(firstNewAddedNode, dom);
            removeNode(toReplaceNodeID, dom);
            return domStatus;
        }

        if (mergeResult == FAILED_MERGE) {
            return DOM_NO_ADD;
        }
    }

    return updateReferences(toReplaceNodeID, firstNewAddedNode, dom);
}
