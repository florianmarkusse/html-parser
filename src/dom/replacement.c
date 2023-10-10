
#include <string.h>

#include "flo/html-parser/dom/deletion.h"
#include "flo/html-parser/dom/dom-util.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/modification.h"
#include "flo/html-parser/dom/query/query-status.h"
#include "flo/html-parser/dom/query/query.h"
#include "flo/html-parser/dom/registry.h"
#include "flo/html-parser/dom/replacement.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/dom/writing.h"
#include "flo/html-parser/node/node.h"
#include "flo/html-parser/node/parent-child.h"
#include "flo/html-parser/parser.h"
#include "flo/html-parser/util/error.h"
#include "flo/html-parser/util/file/read.h"

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
    flo_html_Dom *dom, flo_html_TextStore *textStore, flo_html_Arena *perm) {
    flo_html_String content;
    flo_html_FileStatus fileStatus =
        flo_html_readFile(fileLocation, &content, perm);
    if (fileStatus != FILE_SUCCESS) {
        FLO_HTML_ERROR_WITH_CODE_FORMAT(flo_html_fileStatusToString(fileStatus),
                                        "Failed to read file: \"%s\"",
                                        fileLocation.buf);
        return DOM_ERROR_MEMORY;
    }

    REPLACE_USING_QUERYSELECTOR(cssQuery, content, dom, textStore,
                                flo_html_replaceWithHTMLFromString);
}

static void updateReferences(const flo_html_node_id toReplaceNodeID,
                             const flo_html_node_id newNodeID,
                             flo_html_Dom *dom) {
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
        return;
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
            flo_html_addParentChild(parentFirstChildNode->parentID,
                                    otherNewNodeID, dom);
            otherNewNodeID = flo_html_getNext(otherNewNodeID, dom);
        }
    }

    if (nextNode != NULL) {
        nextNode->currentNodeID = lastNextOfNew;
    }

    nodeToReplace->nodeType = NODE_TYPE_REMOVED;
}

void flo_html_replaceWithDocumentNode(flo_html_node_id toReplaceNodeID,
                                      const flo_html_DocumentNode *docNode,
                                      flo_html_Dom *dom,
                                      flo_html_TextStore *textStore) {
    flo_html_node_id newNodeID =
        flo_html_parseDocumentElement(docNode, dom, textStore);
    updateReferences(toReplaceNodeID, newNodeID, dom);
}

bool tryMergeBothSides(const flo_html_node_id toReplaceNodeID,
                       const flo_html_node_id replacingNodeID,
                       flo_html_Dom *dom, flo_html_TextStore *textStore) {
    flo_html_Node *replacingNode = &dom->nodes[replacingNodeID];
    if (replacingNode->nodeType == NODE_TYPE_TEXT) {
        flo_html_NextNode *previousNode =
            flo_html_getPreviousNode(toReplaceNodeID, dom);
        if (previousNode != NULL) {
            bool isMerged =
                flo_html_tryMerge(&dom->nodes[previousNode->currentNodeID],
                                  replacingNode, dom, textStore, true);
            if (isMerged) {
                return isMerged;
            }
        }

        flo_html_NextNode *nextNode =
            flo_html_getNextNode(toReplaceNodeID, dom);
        if (nextNode != NULL) {
            return flo_html_tryMerge(&dom->nodes[nextNode->nextNodeID],
                                     replacingNode, dom, textStore, false);
        }
    }

    return false;
}

void flo_html_replaceWithTextNode(flo_html_node_id toReplaceNodeID,
                                  const flo_html_String text, flo_html_Dom *dom,
                                  flo_html_TextStore *textStore) {
    flo_html_node_id newNodeID =
        flo_html_parseTextElement(text, dom, textStore);

    bool isMerged =
        tryMergeBothSides(toReplaceNodeID, newNodeID, dom, textStore);
    if (isMerged) {
        flo_html_removeNode(newNodeID, dom);
        flo_html_removeNode(toReplaceNodeID, dom);
        return;
    }

    updateReferences(toReplaceNodeID, newNodeID, dom);
}

void flo_html_replaceWithHTMLFromString(flo_html_node_id toReplaceNodeID,
                                        const flo_html_String htmlString,
                                        flo_html_Dom *dom,
                                        flo_html_TextStore *textStore,
                                        flo_html_Arena *perm) {
    flo_html_node_id firstNewAddedNode = dom->nodeLen;
    flo_html_parseExtra(htmlString, dom, textStore, perm);
    flo_html_node_id lastNextNode =
        flo_html_getLastNext(firstNewAddedNode, dom);
    if (lastNextNode > firstNewAddedNode) {
        flo_html_Node *firstAddedNode = &dom->nodes[firstNewAddedNode];
        if (firstAddedNode->nodeType == NODE_TYPE_TEXT) {
            flo_html_NextNode *previousNode =
                flo_html_getPreviousNode(toReplaceNodeID, dom);
            if (previousNode != NULL) {
                bool isMerged =
                    flo_html_tryMerge(&dom->nodes[previousNode->currentNodeID],
                                      firstAddedNode, dom, textStore, true);
                if (isMerged) {
                    ptrdiff_t secondNewAddedNode =
                        flo_html_getNext(firstNewAddedNode, dom);
                    flo_html_removeNode(firstNewAddedNode, dom);
                    firstNewAddedNode = secondNewAddedNode;
                }
            }
        }

        flo_html_Node *lastAddedNode = &dom->nodes[lastNextNode];
        if (lastAddedNode->nodeType == NODE_TYPE_TEXT) {
            flo_html_NextNode *nextNode =
                flo_html_getNextNode(toReplaceNodeID, dom);
            if (nextNode != NULL) {
                bool isMerged = flo_html_tryMerge(
                    &dom->nodes[nextNode->nextNodeID],
                    &dom->nodes[lastNextNode], dom, textStore, false);
                if (isMerged) {
                    flo_html_removeNode(lastNextNode, dom);
                }
            }
        }
    } else {
        bool isMerged = tryMergeBothSides(toReplaceNodeID, firstNewAddedNode,
                                          dom, textStore);
        if (isMerged) {
            flo_html_removeNode(firstNewAddedNode, dom);
            flo_html_removeNode(toReplaceNodeID, dom);
            return;
        }
    }

    updateReferences(toReplaceNodeID, firstNewAddedNode, dom);
}
