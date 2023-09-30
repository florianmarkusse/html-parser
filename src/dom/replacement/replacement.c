
#include <string.h>

#include "flo/html-parser/dom/deletion/deletion.h"
#include "flo/html-parser/dom/dom-utils.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/modification/modification.h"
#include "flo/html-parser/dom/query/query-status.h"
#include "flo/html-parser/dom/query/query.h"
#include "flo/html-parser/dom/registry.h"
#include "flo/html-parser/dom/replacement/replacement.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/dom/writing.h"
#include "flo/html-parser/parser/parser.h"
#include "flo/html-parser/type/node/node.h"
#include "flo/html-parser/type/node/parent-child.h"
#include "flo/html-parser/utils/file/read.h"
#include "flo/html-parser/utils/print/error.h"

#define REPLACE_USING_QUERYSELECTOR(cssQuery, nodeData, dom, textStore,        \
                                    replaceWithFunction)                       \
    do {                                                                       \
        flo_html_node_id parentNodeID = 0;                                     \
        flo_html_QueryStatus queryResult =                                     \
            flo_html_querySelector(cssQuery, dom, textStore, &parentNodeID);   \
        if (queryResult != QUERY_SUCCESS) {                                    \
            FLO_HTML_PRINT_ERROR(                                              \
                "Could not find element using query selector: %s\n",           \
                (cssQuery).buf);                                               \
            return DOM_NO_ELEMENT;                                             \
        }                                                                      \
        return replaceWithFunction(parentNodeID, nodeData, dom, textStore);    \
    } while (0)

flo_html_DomStatus flo_html_replaceWithDocumentNodeWithQuery(
    const flo_html_String cssQuery, const flo_html_DocumentNode *docNode,
    flo_html_Dom *dom, flo_html_TextStore *textStore) {
    REPLACE_USING_QUERYSELECTOR(cssQuery, docNode, dom, textStore,
                                flo_html_replaceWithDocumentNode);
}

flo_html_DomStatus flo_html_replaceWithTextNodeWithQuery(
    const flo_html_String cssQuery, const flo_html_String text,
    flo_html_Dom *dom, flo_html_TextStore *textStore) {
    REPLACE_USING_QUERYSELECTOR(cssQuery, text, dom, textStore,
                                flo_html_replaceWithTextNode);
}

flo_html_DomStatus flo_html_replaceWithHTMLFromStringWithQuery(
    const flo_html_String cssQuery, const flo_html_String htmlString,
    flo_html_Dom *dom, flo_html_TextStore *textStore) {
    REPLACE_USING_QUERYSELECTOR(cssQuery, htmlString, dom, textStore,
                                flo_html_replaceWithHTMLFromString);
}

flo_html_DomStatus flo_html_replaceWithHTMLFromFileWithQuery(
    const flo_html_String cssQuery, const flo_html_String fileLocation,
    flo_html_Dom *dom, flo_html_TextStore *textStore) {
    flo_html_String content;
    flo_html_FileStatus fileStatus = flo_html_readFile(fileLocation, &content);
    if (fileStatus != FILE_SUCCESS) {
        FLO_HTML_ERROR_WITH_CODE_FORMAT(flo_html_fileStatusToString(fileStatus),
                                        "Failed to read file: \"%s\"",
                                        fileLocation.buf);
        return DOM_ERROR_MEMORY;
    }

    REPLACE_USING_QUERYSELECTOR(cssQuery, content, dom, textStore,
                                flo_html_replaceWithHTMLFromString);
}

static flo_html_DomStatus
updateReferences(const flo_html_node_id toReplaceNodeID,
                 const flo_html_node_id newNodeID, flo_html_Dom *dom) {
    flo_html_Node *nodeToReplace = &dom->nodes[toReplaceNodeID];
    const flo_html_node_id lastNextOfNew = flo_html_getLastNext(newNodeID, dom);

    if (dom->firstNodeID == toReplaceNodeID) {
        flo_html_NextNode *nextNode =
            flo_html_getNextNode(toReplaceNodeID, dom);
        if (nextNode != NULL) {
            nextNode->currentNodeID = lastNextOfNew;
        }

        dom->firstNodeID = newNodeID;
        nodeToReplace->nodeType = NODE_TYPE_REMOVED;
        return DOM_SUCCESS;
    }

    flo_html_ParentChild *parentChildNode =
        flo_html_getParentNode(toReplaceNodeID, dom);
    flo_html_ParentChild *parentFirstChildNode = NULL;
    if (parentChildNode != NULL) {
        parentFirstChildNode =
            flo_html_getFirstChildNode(parentChildNode->parentID, dom);
    }

    flo_html_NextNode *previousNext =
        flo_html_getPreviousNode(toReplaceNodeID, dom);
    flo_html_NextNode *nextNode = flo_html_getNextNode(toReplaceNodeID, dom);

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
        flo_html_node_id otherNewNodeID = flo_html_getNext(newNodeID, dom);
        while (otherNewNodeID > 0) {
            flo_html_DomStatus domStatus = flo_html_addParentChild(
                parentFirstChildNode->parentID, otherNewNodeID, dom);
            if (domStatus != DOM_SUCCESS) {
                FLO_HTML_PRINT_ERROR("Failed to add new node ID as child!\n");
                return domStatus;
            }
            otherNewNodeID = flo_html_getNext(otherNewNodeID, dom);
        }
    }

    if (nextNode != NULL) {
        nextNode->currentNodeID = lastNextOfNew;
    }

    nodeToReplace->nodeType = NODE_TYPE_REMOVED;

    return DOM_SUCCESS;
}

flo_html_DomStatus flo_html_replaceWithDocumentNode(
    flo_html_node_id toReplaceNodeID, const flo_html_DocumentNode *docNode,
    flo_html_Dom *dom, flo_html_TextStore *textStore) {
    flo_html_node_id newNodeID = 0;
    flo_html_DomStatus domStatus =
        flo_html_parseDocumentElement(docNode, dom, textStore, &newNodeID);
    if (domStatus != DOM_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Failed to parse document element!\n");
        return domStatus;
    }
    return updateReferences(toReplaceNodeID, newNodeID, dom);
}

flo_html_MergeResult tryMergeBothSides(const flo_html_node_id toReplaceNodeID,
                                       const flo_html_node_id replacingNodeID,
                                       flo_html_Dom *dom,
                                       flo_html_TextStore *textStore) {
    flo_html_Node *replacingNode = &dom->nodes[replacingNodeID];
    if (replacingNode->nodeType == NODE_TYPE_TEXT) {
        flo_html_NextNode *previousNode =
            flo_html_getPreviousNode(toReplaceNodeID, dom);
        if (previousNode != NULL) {
            flo_html_MergeResult mergeTry =
                flo_html_tryMerge(&dom->nodes[previousNode->currentNodeID],
                                  replacingNode, dom, textStore, true);
            if (mergeTry == FAILED_MERGE || mergeTry == COMPLETED_MERGE) {
                return mergeTry;
            }
        }

        flo_html_NextNode *nextNode =
            flo_html_getNextNode(toReplaceNodeID, dom);
        if (nextNode != NULL) {
            flo_html_MergeResult mergeTry =
                flo_html_tryMerge(&dom->nodes[nextNode->nextNodeID],
                                  replacingNode, dom, textStore, false);
            return mergeTry;
        }
    }

    return NO_MERGE;
}

flo_html_DomStatus
flo_html_replaceWithTextNode(flo_html_node_id toReplaceNodeID,
                             const flo_html_String text, flo_html_Dom *dom,
                             flo_html_TextStore *textStore) {
    flo_html_node_id newNodeID = 0;
    flo_html_DomStatus domStatus =
        flo_html_parseTextElement(text, dom, textStore, &newNodeID);
    if (domStatus != DOM_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Failed to parse text element!\n");
        return domStatus;
    }

    flo_html_MergeResult mergeResult =
        tryMergeBothSides(toReplaceNodeID, newNodeID, dom, textStore);
    if (mergeResult == COMPLETED_MERGE) {
        flo_html_removeNode(newNodeID, dom);
        flo_html_removeNode(toReplaceNodeID, dom);
        return domStatus;
    }

    if (mergeResult == FAILED_MERGE) {
        return DOM_NO_ADD;
    }

    return updateReferences(toReplaceNodeID, newNodeID, dom);
}

flo_html_DomStatus flo_html_replaceWithHTMLFromString(
    flo_html_node_id toReplaceNodeID, const flo_html_String htmlString,
    flo_html_Dom *dom, flo_html_TextStore *textStore) {
    flo_html_node_id firstNewAddedNode = dom->nodeLen;
    flo_html_DomStatus domStatus = flo_html_parse(htmlString, dom, textStore);
    if (domStatus != DOM_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Failed to parse string!\n");
        return domStatus;
    }

    flo_html_node_id lastNextNode =
        flo_html_getLastNext(firstNewAddedNode, dom);
    if (lastNextNode > firstNewAddedNode) {
        flo_html_Node *firstAddedNode = &dom->nodes[firstNewAddedNode];
        if (firstAddedNode->nodeType == NODE_TYPE_TEXT) {
            flo_html_NextNode *previousNode =
                flo_html_getPreviousNode(toReplaceNodeID, dom);
            if (previousNode != NULL) {
                flo_html_MergeResult mergeResult =
                    flo_html_tryMerge(&dom->nodes[previousNode->currentNodeID],
                                      firstAddedNode, dom, textStore, true);
                if (mergeResult == COMPLETED_MERGE) {
                    size_t secondNewAddedNode =
                        flo_html_getNext(firstNewAddedNode, dom);
                    flo_html_removeNode(firstNewAddedNode, dom);
                    firstNewAddedNode = secondNewAddedNode;
                }

                if (mergeResult == FAILED_MERGE) {
                    return DOM_NO_ADD;
                }
            }
        }

        flo_html_Node *lastAddedNode = &dom->nodes[lastNextNode];
        if (lastAddedNode->nodeType == NODE_TYPE_TEXT) {
            flo_html_NextNode *nextNode =
                flo_html_getNextNode(toReplaceNodeID, dom);
            if (nextNode != NULL) {
                flo_html_MergeResult mergeResult = flo_html_tryMerge(
                    &dom->nodes[nextNode->nextNodeID],
                    &dom->nodes[lastNextNode], dom, textStore, false);

                if (mergeResult == COMPLETED_MERGE) {
                    flo_html_removeNode(lastNextNode, dom);
                }

                if (mergeResult == FAILED_MERGE) {
                    return DOM_NO_ADD;
                }
            }
        }
    } else {
        flo_html_MergeResult mergeResult = tryMergeBothSides(
            toReplaceNodeID, firstNewAddedNode, dom, textStore);
        if (mergeResult == COMPLETED_MERGE) {
            flo_html_removeNode(firstNewAddedNode, dom);
            flo_html_removeNode(toReplaceNodeID, dom);
            return domStatus;
        }

        if (mergeResult == FAILED_MERGE) {
            return DOM_NO_ADD;
        }
    }

    return updateReferences(toReplaceNodeID, firstNewAddedNode, dom);
}
