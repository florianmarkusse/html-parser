
#include <string.h>

#include "flo/html-parser/dom/deletion.h"
#include "flo/html-parser/dom/dom-util.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/modification.h"
#include "flo/html-parser/dom/query/query-status.h"
#include "flo/html-parser/dom/query/query.h"
#include "flo/html-parser/dom/replacement.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/dom/writing.h"
#include "flo/html-parser/node/node.h"
#include "flo/html-parser/node/parent-child.h"
#include "flo/html-parser/parser.h"
#include "flo/html-parser/util/error.h"
#include "flo/html-parser/util/file/read.h"

#define REPLACE_USING_QUERYSELECTOR(cssQuery, nodeData, dom, perm,             \
                                    replaceWithFunction)                       \
    do {                                                                       \
        flo_html_node_id parentNodeID = 0;                                     \
        flo_html_QueryStatus queryResult =                                     \
            flo_html_querySelector(cssQuery, dom, &parentNodeID, *(perm));     \
        if (queryResult != QUERY_SUCCESS) {                                    \
            FLO_HTML_PRINT_ERROR(                                              \
                "Could not find element using query selector: %s\n",           \
                (cssQuery).buf);                                               \
            return 0;                                                          \
        }                                                                      \
        return replaceWithFunction(parentNodeID, nodeData, dom, perm);         \
    } while (0)

flo_html_node_id flo_html_replaceWithDocumentNodeWithQuery(
    flo_html_String cssQuery, flo_html_DocumentNode *docNode, flo_html_Dom *dom,
    flo_html_Arena *perm) {
    REPLACE_USING_QUERYSELECTOR(cssQuery, docNode, dom, perm,
                                flo_html_replaceWithDocumentNode);
}

flo_html_node_id flo_html_replaceWithTextNodeWithQuery(flo_html_String cssQuery,
                                                       flo_html_String text,
                                                       flo_html_Dom *dom,
                                                       flo_html_Arena *perm) {
    REPLACE_USING_QUERYSELECTOR(cssQuery, text, dom, perm,
                                flo_html_replaceWithTextNode);
}

flo_html_node_id flo_html_replaceWithHTMLFromStringWithQuery(
    flo_html_String cssQuery, flo_html_String htmlString, flo_html_Dom *dom,
    flo_html_Arena *perm) {
    REPLACE_USING_QUERYSELECTOR(cssQuery, htmlString, dom, perm,
                                flo_html_replaceWithHTMLFromString);
}

flo_html_node_id flo_html_replaceWithHTMLFromFileWithQuery(
    flo_html_String cssQuery, flo_html_String fileLocation, flo_html_Dom *dom,
    flo_html_Arena *perm) {
    flo_html_String content;
    flo_html_FileStatus fileStatus =
        flo_html_readFile(fileLocation, &content, perm);
    if (fileStatus != FILE_SUCCESS) {
        FLO_HTML_ERROR_WITH_CODE_FORMAT(flo_html_fileStatusToString(fileStatus),
                                        "Failed to read file: \"%s\"",
                                        fileLocation.buf);
        return FLO_HTML_ERROR_NODE_ID;
    }

    REPLACE_USING_QUERYSELECTOR(cssQuery, content, dom, perm,
                                flo_html_replaceWithHTMLFromString);
}

static void updateReferences(flo_html_node_id toReplaceNodeID,
                             flo_html_node_id newNodeID, flo_html_Dom *dom,
                             flo_html_Arena *perm) {
    flo_html_Node *nodeToReplace = &dom->nodes.buf[toReplaceNodeID];
    flo_html_node_id lastNextOfNew = flo_html_getLastNext(newNodeID, dom);

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
            *FLO_HTML_PUSH(&dom->parentChilds, perm) =
                (flo_html_ParentChild){.parentID = parentChildNode->parentID,
                                       .childID = otherNewNodeID};
            otherNewNodeID = flo_html_getNext(otherNewNodeID, dom);
        }
    }

    if (nextNode != NULL) {
        nextNode->currentNodeID = lastNextOfNew;
    }

    nodeToReplace->nodeType = NODE_TYPE_REMOVED;
}

flo_html_node_id
flo_html_replaceWithDocumentNode(flo_html_node_id toReplaceNodeID,
                                 flo_html_DocumentNode *docNode,
                                 flo_html_Dom *dom, flo_html_Arena *perm) {
    flo_html_node_id newNodeID =
        flo_html_parseDocumentElement(docNode, dom, perm);
    if (newNodeID == 0) {
        return 0;
    }
    updateReferences(toReplaceNodeID, newNodeID, dom, perm);
    return newNodeID;
}

bool tryMergeBothSides(flo_html_node_id toReplaceNodeID,
                       flo_html_node_id replacingNodeID, flo_html_Dom *dom,
                       flo_html_Arena *perm) {
    flo_html_Node *replacingNode = &dom->nodes.buf[replacingNodeID];
    if (replacingNode->nodeType == NODE_TYPE_TEXT) {
        flo_html_NextNode *previousNode =
            flo_html_getPreviousNode(toReplaceNodeID, dom);
        if (previousNode != NULL) {
            bool isMerged =
                flo_html_tryMerge(previousNode->currentNodeID,
                                  replacingNode->nodeID, dom, true, perm);
            if (isMerged) {
                return isMerged;
            }
        }

        flo_html_NextNode *nextNode =
            flo_html_getNextNode(toReplaceNodeID, dom);
        if (nextNode != NULL) {
            return flo_html_tryMerge(nextNode->nextNodeID,
                                     replacingNode->nodeID, dom, false, perm);
        }
    }

    return false;
}

flo_html_node_id flo_html_replaceWithTextNode(flo_html_node_id toReplaceNodeID,
                                              flo_html_String text,
                                              flo_html_Dom *dom,
                                              flo_html_Arena *perm) {
    flo_html_node_id newNodeID = flo_html_parseTextElement(text, dom, perm);
    if (newNodeID == 0) {
        return 0;
    }

    bool isMerged = tryMergeBothSides(toReplaceNodeID, newNodeID, dom, perm);
    if (isMerged) {
        flo_html_removeNode(newNodeID, dom);
        flo_html_removeNode(toReplaceNodeID, dom);
        return newNodeID;
    }

    updateReferences(toReplaceNodeID, newNodeID, dom, perm);
    return newNodeID;
}

flo_html_node_id
flo_html_replaceWithHTMLFromString(flo_html_node_id toReplaceNodeID,
                                   flo_html_String htmlString,
                                   flo_html_Dom *dom, flo_html_Arena *perm) {
    flo_html_node_id firstNewAddedNode = (flo_html_node_id)dom->nodes.len;
    dom = flo_html_parseExtra(htmlString, dom, perm);
    if (dom == NULL) {
        return 0;
    }

    flo_html_node_id lastNextNode =
        flo_html_getLastNext(firstNewAddedNode, dom);

    flo_html_NextNode *previousNode =
        flo_html_getPreviousNode(toReplaceNodeID, dom);
    if (lastNextNode > firstNewAddedNode) {
        flo_html_Node *firstAddedNode = &dom->nodes.buf[firstNewAddedNode];
        if (firstAddedNode->nodeType == NODE_TYPE_TEXT) {
            if (previousNode != NULL) {
                if (flo_html_tryMerge(previousNode->currentNodeID,
                                      firstAddedNode->nodeID, dom, true,
                                      perm)) {
                    flo_html_node_id secondNewAddedNode =
                        flo_html_getNext(firstNewAddedNode, dom);

                    flo_html_removeNode(firstNewAddedNode, dom);
                    firstNewAddedNode = secondNewAddedNode;
                }
            }
        }

        flo_html_Node *lastAddedNode = &dom->nodes.buf[lastNextNode];
        if (lastAddedNode->nodeType == NODE_TYPE_TEXT) {
            flo_html_NextNode *nextNode =
                flo_html_getNextNode(toReplaceNodeID, dom);
            if (nextNode != NULL) {
                if (flo_html_tryMerge(nextNode->nextNodeID, lastNextNode, dom,
                                      false, perm)) {
                    flo_html_removeNode(lastNextNode, dom);
                }
            }
        }
    } else {
        bool isMerged =
            tryMergeBothSides(toReplaceNodeID, firstNewAddedNode, dom, perm);
        if (isMerged) {
            flo_html_removeNode(firstNewAddedNode, dom);
            flo_html_removeNode(toReplaceNodeID, dom);
            return previousNode->currentNodeID;
        }
    }

    updateReferences(toReplaceNodeID, firstNewAddedNode, dom, perm);

    return firstNewAddedNode;
}
