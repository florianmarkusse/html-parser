#include <string.h>

#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/modification/modification.h"
#include "flo/html-parser/dom/prependix/prependix.h"
#include "flo/html-parser/dom/registry.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/parser/parser.h"
#include "flo/html-parser/type/node/node.h"
#include "flo/html-parser/type/node/parent-child.h"
#include "flo/html-parser/utils/print/error.h"

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

        return domStatus;
    }

    node_id previousFirstChild = firstChild->childID;
    firstChild->childID = firstNewNodeID;

    domStatus = addNextNode(firstNewNodeID, previousFirstChild, dom);
    if (domStatus != DOM_SUCCESS) {
        PRINT_ERROR("Failed to add new node ID in next nodes!\n");
        return domStatus;
    }

    domStatus = addParentChild(parentID, firstNewNodeID, dom);
    if (domStatus != DOM_SUCCESS) {
        PRINT_ERROR("Failed to add new node ID as child!\n");
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

    return updateReferences(parentID, newNodeID, dom);
}

DomStatus prependNodesFromString(const node_id parentID, const char *htmlString,
                                 Dom *dom, DataContainer *dataContainer) {
    node_id firstNewAddedNode = dom->nodeLen;
    DomStatus domStatus = parse(htmlString, dom, dataContainer);
    if (domStatus != DOM_SUCCESS) {
        PRINT_ERROR("Failed to parse string!\n");
        return domStatus;
    }

    return updateReferences(parentID, firstNewAddedNode, dom);
}
