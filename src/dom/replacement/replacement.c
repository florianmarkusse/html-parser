
#include <string.h>

#include "flo/html-parser/dom/deletion/deletion.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/modification/modification.h"
#include "flo/html-parser/dom/registry.h"
#include "flo/html-parser/dom/replacement/replacement.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/parser/parser.h"
#include "flo/html-parser/type/node/parent-child.h"
#include "flo/html-parser/utils/print/error.h"

static DomStatus updateReferences(const node_id toReplaceNodeID,
                                  const node_id newNodeID, Dom *dom) {
    NextNode *previousNext = getPreviousNode(toReplaceNodeID, dom);
    node_id currentNodeID = 0;
    if (previousNext != NULL) {
        currentNodeID = previousNext->currentNodeID;
    }
    NextNode *nextNode = getNextNode(previousNext->nextNodeID, dom);
    node_id nextNodeID = 0;
    if (previousNext != NULL) {
        nextNodeID = previousNext->nextNodeID;
    }

    removeNode(toReplaceNodeID, dom);

    if (previousNext != NULL) {
        previousNext->currentNodeID = currentNodeID;
        previousNext->nextNodeID = newNodeID;
    }

    if (nextNode != NULL) {
        nextNode->currentNodeID = newNodeID;
        nextNode->nextNodeID = nextNodeID;
    }

    return DOM_SUCCESS;
}

DomStatus replaceWithDocumentNode(node_id parentID, const DocumentNode *docNode,
                                  Dom *dom, DataContainer *dataContainer) {
    node_id newNodeID = 0;
    DomStatus domStatus =
        parseDocumentElement(docNode, dom, dataContainer, &newNodeID);
    if (domStatus != DOM_SUCCESS) {
        PRINT_ERROR("Failed to parse document element!\n");
        return domStatus;
    }
    updateReferences(parentID, newNodeID, dom);
    return domStatus;
}
DomStatus replaceWithTextNode(node_id parentID, const char *text, Dom *dom,
                              DataContainer *dataContainer) {
    node_id newNodeID = 0;
    DomStatus domStatus =
        parseTextElement(text, dom, dataContainer, &newNodeID);
    if (domStatus != DOM_SUCCESS) {
        PRINT_ERROR("Failed to parse text element!\n");
        return domStatus;
    }

    updateReferences(parentID, newNodeID, dom);
    return domStatus;
}
DomStatus replaceWithNodesFromString(node_id parentID, const char *htmlString,
                                     Dom *dom, DataContainer *dataContainer) {
    node_id firstNewAddedNode = dom->nodeLen;
    DomStatus domStatus = parse(htmlString, dom, dataContainer);
    if (domStatus != DOM_SUCCESS) {
        PRINT_ERROR("Failed to parse string!\n");
        return domStatus;
    }

    updateReferences(parentID, firstNewAddedNode, dom);
    return domStatus;
}
