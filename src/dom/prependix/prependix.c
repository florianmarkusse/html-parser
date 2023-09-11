#include <string.h>

#include "flo/html-parser/dom/deletion/deletion.h"
#include "flo/html-parser/dom/dom-utils.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/modification/modification.h"
#include "flo/html-parser/dom/prependix/prependix.h"
#include "flo/html-parser/dom/query/query-status.h"
#include "flo/html-parser/dom/query/query.h"
#include "flo/html-parser/dom/registry.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/dom/writing.h"
#include "flo/html-parser/parser/parser.h"
#include "flo/html-parser/type/node/node.h"
#include "flo/html-parser/type/node/parent-child.h"
#include "flo/html-parser/utils/file/read.h"
#include "flo/html-parser/utils/print/error.h"

#define PREPEND_USING_QUERYSELECTOR(cssQuery, nodeData, dom, dataContainer,    \
                                    prependFunction)                           \
    do {                                                                       \
        node_id parentNodeID = 0;                                              \
        QueryStatus queryResult =                                              \
            querySelector(cssQuery, dom, dataContainer, &parentNodeID);        \
        if (queryResult != QUERY_SUCCESS) {                                    \
            PRINT_ERROR("Could not find element using query selector: %s\n",   \
                        cssQuery);                                             \
            return DOM_NO_ELEMENT;                                             \
        }                                                                      \
        return prependFunction(parentNodeID, nodeData, dom, dataContainer);    \
    } while (0)

DomStatus prependDocumentNodeWithQuery(const char *cssQuery,
                                       const DocumentNode *docNode, Dom *dom,
                                       DataContainer *dataContainer) {
    PREPEND_USING_QUERYSELECTOR(cssQuery, docNode, dom, dataContainer,
                                prependDocumentNode);
}

DomStatus prependTextNodeWithQuery(const char *cssQuery, const char *text,
                                   Dom *dom, DataContainer *dataContainer) {
    PREPEND_USING_QUERYSELECTOR(cssQuery, text, dom, dataContainer,
                                prependTextNode);
}

DomStatus prependHTMLFromStringWithQuery(const char *cssQuery,
                                         const char *htmlString, Dom *dom,
                                         DataContainer *dataContainer) {
    PREPEND_USING_QUERYSELECTOR(cssQuery, htmlString, dom, dataContainer,
                                prependHTMLFromString);
}

DomStatus prependHTMLFromFileWithQuery(const char *cssQuery,
                                       const char *fileLocation, Dom *dom,
                                       DataContainer *dataContainer) {
    char *buffer = NULL;
    FileStatus fileStatus = readFile(fileLocation, &buffer);
    if (fileStatus != FILE_SUCCESS) {
        ERROR_WITH_CODE_FORMAT(fileStatusToString(fileStatus),
                               "Failed to read file: \"%s\"", fileLocation);
        return DOM_ERROR_MEMORY;
    }

    PREPEND_USING_QUERYSELECTOR(cssQuery, buffer, dom, dataContainer,
                                prependHTMLFromString);
}

static DomStatus updateReferences(const node_id parentID,
                                  const node_id firstNewNodeID, Dom *dom) {
    DomStatus domStatus = DOM_SUCCESS;
    if (parentID == 0) {
        node_id previousFirstNodeID = dom->firstNodeID;
        domStatus = addNextNode(firstNewNodeID, dom->firstNodeID, dom);
        if (domStatus != DOM_SUCCESS) {
            PRINT_ERROR("Failed to add new node ID in next nodes!\n");
            return domStatus;
        }
        dom->firstNodeID = firstNewNodeID;

        node_id lastNextNode = getLastNext(dom->firstNodeID, dom);
        if (lastNextNode > firstNewNodeID) {
            domStatus = addNextNode(lastNextNode, previousFirstNodeID, dom);
            if (domStatus != DOM_SUCCESS) {
                PRINT_ERROR("Failed to add new node ID in next nodes!\n");
                return domStatus;
            }
        }

        return domStatus;
    }

    ParentChild *firstChild = getFirstChildNode(parentID, dom);
    if (firstChild == NULL) {
        domStatus = addParentFirstChild(parentID, firstNewNodeID, dom);
        if (domStatus != DOM_SUCCESS) {
            PRINT_ERROR("Failed to add new node ID as first child!\n");
            return domStatus;
        }

        domStatus = addParentChild(parentID, firstNewNodeID, dom);
        if (domStatus != DOM_SUCCESS) {
            PRINT_ERROR("Failed to add new node ID as child!\n");
            return domStatus;
        }

        domStatus = connectOtherNodesToParent(parentID, firstNewNodeID, dom);
        if (domStatus != DOM_SUCCESS) {
            PRINT_ERROR("Failed to add remaning node IDs as child!\n");
            return domStatus;
        }

        return domStatus;
    }

    node_id previousFirstChild = firstChild->childID;
    firstChild->childID = firstNewNodeID;

    node_id lastNextOfNew = getLastNext(firstNewNodeID, dom);
    domStatus = addNextNode(lastNextOfNew, previousFirstChild, dom);
    if (domStatus != DOM_SUCCESS) {
        PRINT_ERROR("Failed to add new node ID in next nodes!\n");
        return domStatus;
    }

    domStatus = addParentChild(parentID, firstNewNodeID, dom);
    if (domStatus != DOM_SUCCESS) {
        PRINT_ERROR("Failed to add new node ID as child!\n");
        return domStatus;
    }

    domStatus = connectOtherNodesToParent(parentID, firstNewNodeID, dom);
    if (domStatus != DOM_SUCCESS) {
        PRINT_ERROR("Failed to add remaning node IDs as child!\n");
        return domStatus;
    }

    return domStatus;
}

DomStatus prependDocumentNode(const node_id parentID,
                              const DocumentNode *docNode, Dom *dom,
                              DataContainer *dataContainer) {
    node_id newNodeID = 0;
    DomStatus domStatus =
        parseDocumentElement(docNode, dom, dataContainer, &newNodeID);
    if (domStatus != DOM_SUCCESS) {
        PRINT_ERROR("Failed to parse document element!\n");
        return domStatus;
    }
    return updateReferences(parentID, newNodeID, dom);
}

DomStatus prependTextNode(const node_id parentID, const char *text, Dom *dom,
                          DataContainer *dataContainer) {
    node_id newNodeID = 0;
    DomStatus domStatus =
        parseTextElement(text, dom, dataContainer, &newNodeID);
    if (domStatus != DOM_SUCCESS) {
        PRINT_ERROR("Failed to parse text element!\n");
        return domStatus;
    }

    node_id child = getFirstChild(parentID, dom);
    if (child > 0) {
        MergeResult mergeTry =
            tryMerge(&dom->nodes[child], &dom->nodes[newNodeID], dom,
                     dataContainer, false);

        if (mergeTry == COMPLETED_MERGE) {
            removeNode(newNodeID, dom);
            return domStatus;
        }

        if (mergeTry == FAILED_MERGE) {
            return DOM_NO_ADD;
        }
    }

    return updateReferences(parentID, newNodeID, dom);
}

DomStatus prependHTMLFromString(const node_id parentID, const char *htmlString,
                                 Dom *dom, DataContainer *dataContainer) {
    node_id firstNewAddedNode = dom->nodeLen;
    DomStatus domStatus = parse(htmlString, dom, dataContainer);
    if (domStatus != DOM_SUCCESS) {
        PRINT_ERROR("Failed to parse string!\n");
        return domStatus;
    }

    node_id firstChild = getFirstChild(parentID, dom);
    if (firstChild > 0) {
        node_id lastNextNode = getLastNext(firstNewAddedNode, dom);
        if (lastNextNode > firstNewAddedNode) {
            Node *lastAddedNode = &dom->nodes[lastNextNode];
            if (lastAddedNode->nodeType == NODE_TYPE_TEXT) {
                MergeResult mergeResult =
                    tryMerge(&dom->nodes[firstChild], lastAddedNode, dom,
                             dataContainer, false);
                if (mergeResult == COMPLETED_MERGE) {
                    removeNode(lastNextNode, dom);
                }

                if (mergeResult == FAILED_MERGE) {
                    return DOM_NO_ADD;
                }
            }
        } else {
            Node *firstAddedNode = &dom->nodes[firstNewAddedNode];
            if (firstAddedNode->nodeType == NODE_TYPE_TEXT) {
                MergeResult mergeResult =
                    tryMerge(&dom->nodes[firstChild], firstAddedNode, dom,
                             dataContainer, false);
                if (mergeResult == COMPLETED_MERGE) {
                    removeNode(firstNewAddedNode, dom);
                    return domStatus;
                }

                if (mergeResult == FAILED_MERGE) {
                    return DOM_NO_ADD;
                }
            }
        }
    }

    return updateReferences(parentID, firstNewAddedNode, dom);
}
