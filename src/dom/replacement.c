
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

#define REPLACE_USING_QUERYSELECTOR(cssQuery, nodeData, parsed, perm,          \
                                    replaceWithFunction)                       \
    do {                                                                       \
        flo_html_node_id parentNodeID = 0;                                     \
        flo_html_QueryStatus queryResult =                                     \
            flo_html_querySelector(cssQuery, parsed, &parentNodeID, *(perm));  \
        if (queryResult != QUERY_SUCCESS) {                                    \
            FLO_HTML_PRINT_ERROR(                                              \
                "Could not find element using query selector: %s\n",           \
                (cssQuery).buf);                                               \
            return 0;                                                          \
        }                                                                      \
        return replaceWithFunction(parentNodeID, nodeData, parsed, perm);      \
    } while (0)

flo_html_node_id flo_html_replaceWithDocumentNodeWithQuery(
    const flo_html_String cssQuery, const flo_html_DocumentNode *docNode,
    flo_html_ParsedHTML parsed, flo_html_Arena *perm) {
    REPLACE_USING_QUERYSELECTOR(cssQuery, docNode, parsed, perm,
                                flo_html_replaceWithDocumentNode);
}

flo_html_node_id flo_html_replaceWithTextNodeWithQuery(
    const flo_html_String cssQuery, const flo_html_String text,
    flo_html_ParsedHTML parsed, flo_html_Arena *perm) {
    REPLACE_USING_QUERYSELECTOR(cssQuery, text, parsed, perm,
                                flo_html_replaceWithTextNode);
}

flo_html_node_id flo_html_replaceWithHTMLFromStringWithQuery(
    const flo_html_String cssQuery, const flo_html_String htmlString,
    flo_html_ParsedHTML parsed, flo_html_Arena *perm) {
    REPLACE_USING_QUERYSELECTOR(cssQuery, htmlString, parsed, perm,
                                flo_html_replaceWithHTMLFromString);
}

flo_html_node_id flo_html_replaceWithHTMLFromFileWithQuery(
    const flo_html_String cssQuery, const flo_html_String fileLocation,
    flo_html_ParsedHTML parsed, flo_html_Arena *perm) {
    flo_html_String content;
    flo_html_FileStatus fileStatus =
        flo_html_readFile(fileLocation, &content, perm);
    if (fileStatus != FILE_SUCCESS) {
        FLO_HTML_ERROR_WITH_CODE_FORMAT(flo_html_fileStatusToString(fileStatus),
                                        "Failed to read file: \"%s\"",
                                        fileLocation.buf);
        return 0;
    }

    REPLACE_USING_QUERYSELECTOR(cssQuery, content, parsed, perm,
                                flo_html_replaceWithHTMLFromString);
}

static void updateReferences(const flo_html_node_id toReplaceNodeID,
                             const flo_html_node_id newNodeID,
                             flo_html_Dom *dom) {
    flo_html_Node *nodeToReplace = &dom->nodes[toReplaceNodeID];
    const flo_html_node_id lastNextOfNew = flo_html_getLastNext(newNodeID, dom);

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

flo_html_node_id flo_html_replaceWithDocumentNode(
    flo_html_node_id toReplaceNodeID, const flo_html_DocumentNode *docNode,
    flo_html_ParsedHTML parsed, flo_html_Arena *perm) {
    flo_html_node_id newNodeID = flo_html_parseDocumentElement(docNode, parsed);
    updateReferences(toReplaceNodeID, newNodeID, parsed.dom);
    return newNodeID;
}

bool tryMergeBothSides(const flo_html_node_id toReplaceNodeID,
                       const flo_html_node_id replacingNodeID,
                       flo_html_ParsedHTML parsed) {
    flo_html_Node *replacingNode = &parsed.dom->nodes[replacingNodeID];
    if (replacingNode->nodeType == NODE_TYPE_TEXT) {
        flo_html_NextNode *previousNode =
            flo_html_getPreviousNode(toReplaceNodeID, parsed.dom);
        if (previousNode != NULL) {
            bool isMerged = flo_html_tryMerge(
                &parsed.dom->nodes[previousNode->currentNodeID], replacingNode,
                parsed, true);
            if (isMerged) {
                return isMerged;
            }
        }

        flo_html_NextNode *nextNode =
            flo_html_getNextNode(toReplaceNodeID, parsed.dom);
        if (nextNode != NULL) {
            return flo_html_tryMerge(&parsed.dom->nodes[nextNode->nextNodeID],
                                     replacingNode, parsed, false);
        }
    }

    return false;
}

flo_html_node_id flo_html_replaceWithTextNode(flo_html_node_id toReplaceNodeID,
                                              const flo_html_String text,
                                              flo_html_ParsedHTML parsed,
                                              flo_html_Arena *perm) {
    flo_html_node_id newNodeID = flo_html_parseTextElement(text, parsed);

    bool isMerged = tryMergeBothSides(toReplaceNodeID, newNodeID, parsed);
    if (isMerged) {
        flo_html_removeNode(newNodeID, parsed.dom);
        flo_html_removeNode(toReplaceNodeID, parsed.dom);
        return newNodeID;
    }

    updateReferences(toReplaceNodeID, newNodeID, parsed.dom);
    return newNodeID;
}

flo_html_node_id flo_html_replaceWithHTMLFromString(
    flo_html_node_id toReplaceNodeID, const flo_html_String htmlString,
    flo_html_ParsedHTML parsed, flo_html_Arena *perm) {
    flo_html_node_id firstNewAddedNode = parsed.dom->nodeLen;
    flo_html_parseExtra(htmlString, parsed, perm);

    flo_html_node_id lastNextNode =
        flo_html_getLastNext(firstNewAddedNode, parsed.dom);

    flo_html_NextNode *previousNode =
        flo_html_getPreviousNode(toReplaceNodeID, parsed.dom);
    if (lastNextNode > firstNewAddedNode) {
        flo_html_Node *firstAddedNode = &parsed.dom->nodes[firstNewAddedNode];
        if (firstAddedNode->nodeType == NODE_TYPE_TEXT) {
            if (previousNode != NULL) {
                if (flo_html_tryMerge(
                        &parsed.dom->nodes[previousNode->currentNodeID],
                        firstAddedNode, parsed, true)) {
                    ptrdiff_t secondNewAddedNode =
                        flo_html_getNext(firstNewAddedNode, parsed.dom);

                    flo_html_removeNode(firstNewAddedNode, parsed.dom);
                    firstNewAddedNode = secondNewAddedNode;
                }
            }
        }

        flo_html_Node *lastAddedNode = &parsed.dom->nodes[lastNextNode];
        if (lastAddedNode->nodeType == NODE_TYPE_TEXT) {
            flo_html_NextNode *nextNode =
                flo_html_getNextNode(toReplaceNodeID, parsed.dom);
            if (nextNode != NULL) {
                if (flo_html_tryMerge(&parsed.dom->nodes[nextNode->nextNodeID],
                                      &parsed.dom->nodes[lastNextNode], parsed,
                                      false)) {
                    flo_html_removeNode(lastNextNode, parsed.dom);
                }
            }
        }
    } else {
        bool isMerged =
            tryMergeBothSides(toReplaceNodeID, firstNewAddedNode, parsed);
        if (isMerged) {
            flo_html_removeNode(firstNewAddedNode, parsed.dom);
            flo_html_removeNode(toReplaceNodeID, parsed.dom);
            return previousNode->currentNodeID;
        }
    }

    updateReferences(toReplaceNodeID, firstNewAddedNode, parsed.dom);

    return firstNewAddedNode;
}
