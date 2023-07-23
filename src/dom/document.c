#include <stdlib.h>

#include "dom/document.h"
#include "tokenizer/parse.h"
#include "utils/print/error.h"

DocumentStatus createDocument(const char *xmlString, Document *doc) {
    doc->nodes = malloc(NODES_PAGE_SIZE);
    doc->nodeLen = 0;
    doc->nodeCap = NODES_PER_PAGE;

    doc->parentFirstChilds = malloc(PARENT_CHILDS_PAGE_SIZE);
    doc->parentFirstChildLen = 0;
    doc->parentFirstChildCap = PARENT_CHILDS_PER_PAGE;

    doc->nextNodes = malloc(NEXT_NODES_PAGE_SIZE);
    doc->nextNodeLen = 0;
    doc->nextNodeCap = NEXT_NODES_PER_PAGE;

    doc->boolProps = malloc(BOOLEAN_PROPERTIES_PAGE_SIZE);
    doc->boolPropsLen = 0;
    doc->boolPropsCap = BOOLEAN_PROPERTIES_PER_PAGE;

    doc->props = malloc(PROPERTIES_PAGE_SIZE);
    doc->propsLen = 0;
    doc->propsCap = PROPERTIES_PER_PAGE;

    doc->text = malloc(TEXT_NODES_PAGE_SIZE);
    doc->textLen = 0;
    doc->textCap = TEXT_NODES_PER_PAGE;

    if (doc->nodes == NULL || doc->parentFirstChilds == NULL ||
        doc->nextNodes == NULL || doc->boolProps == NULL ||
        doc->props == NULL || doc->text == NULL) {
        PRINT_ERROR("Failed to allocate memory for nodes.\n");
        destroyDocument(doc);
        return DOCUMENT_ERROR_MEMORY;
    }

    DocumentStatus documentStatus = parse(xmlString, doc);
    if (documentStatus != DOCUMENT_SUCCESS) {
        PRINT_ERROR("Failed to parse document.\n");
    }
    return documentStatus;
}

void *resizeArray(void *array, size_t currentLen, size_t *currentCap,
                  size_t elementSize, size_t extraElements) {
    if (currentLen < *currentCap) {
        return array;
    }
    size_t newCap = (currentLen + extraElements) * elementSize;
    void *newArray = realloc(array, newCap);
    if (newArray == NULL) {
        PRINT_ERROR("Failed to reallocate more memory for the array.\n");
        return NULL;
    }
    *currentCap = newCap / elementSize;
    return newArray;
}

DocumentStatus addNode(node_id *nodeID, element_id tagID, Document *doc) {
    if ((doc->nodes = resizeArray(doc->nodes, doc->nodeLen, &doc->nodeCap,
                                  sizeof(Node), NODES_PER_PAGE)) == NULL) {
        return DOCUMENT_ERROR_MEMORY;
    }

    Node *newNode = &(doc->nodes[doc->nodeLen]);
    newNode->nodeID =
        doc->nodeLen + 1; // We start at 1 because 0 is used as error id
    newNode->tagID = tagID;

    doc->nodeLen++;
    *nodeID = newNode->nodeID;
    return DOCUMENT_SUCCESS;
}

DocumentStatus addParentFirstChild(node_id parentID, node_id childID,
                                   Document *doc) {
    if ((doc->parentFirstChilds =
             resizeArray(doc->parentFirstChilds, doc->parentFirstChildLen,
                         &doc->parentFirstChildCap, sizeof(ParentFirstChild),
                         PARENT_CHILDS_PER_PAGE)) == NULL) {
        return DOCUMENT_ERROR_MEMORY;
    }

    ParentFirstChild *newParentFirstChild =
        &(doc->parentFirstChilds[doc->parentFirstChildLen]);
    newParentFirstChild->parentID = parentID;
    newParentFirstChild->childID = childID;
    doc->parentFirstChildLen++;
    return DOCUMENT_SUCCESS;
}

node_id getFirstChild(const node_id parentID, const Document *doc) {
    for (node_id i = 0; i < doc->parentFirstChildLen; i++) {
        if (doc->parentFirstChilds[i].parentID == parentID) {
            return doc->parentFirstChilds[i].childID;
        }
    }
    return 0;
}

DocumentStatus addNextNode(node_id currentNodeID, node_id nextNodeID,
                           Document *doc) {
    if ((doc->nextNodes =
             resizeArray(doc->nextNodes, doc->nextNodeLen, &doc->nextNodeCap,
                         sizeof(NextNode), NEXT_NODES_PER_PAGE)) == NULL) {
        return DOCUMENT_ERROR_MEMORY;
    }

    NextNode *newNextNode = &(doc->nextNodes[doc->nextNodeLen]);
    newNextNode->currentNodeID = currentNodeID;
    newNextNode->nextNodeID = nextNodeID;
    doc->nextNodeLen++;
    return DOCUMENT_SUCCESS;
}

node_id getNextNode(const node_id currentNodeID, const Document *doc) {
    for (node_id i = 0; i < doc->nextNodeLen; i++) {
        if (doc->nextNodes[i].currentNodeID == currentNodeID) {
            return doc->nextNodes[i].nextNodeID;
        }
    }
    return 0;
}

DocumentStatus addBooleanProperty(const node_id nodeID, const element_id propID,
                                  Document *doc) {
    if ((doc->boolProps = resizeArray(
             doc->boolProps, doc->boolPropsLen, &doc->boolPropsCap,
             sizeof(BooleanProperty), BOOLEAN_PROPERTIES_PER_PAGE)) == NULL) {
        return DOCUMENT_ERROR_MEMORY;
    }

    BooleanProperty *newBooleanProperty = &(doc->boolProps[doc->boolPropsLen]);
    newBooleanProperty->nodeID = nodeID;
    newBooleanProperty->propID = propID;
    doc->boolPropsLen++;
    return DOCUMENT_SUCCESS;
}

DocumentStatus addProperty(const node_id nodeID, const element_id keyID,
                           const element_id valueID, Document *doc) {
    if ((doc->props = resizeArray(doc->props, doc->propsLen, &doc->propsCap,
                                  sizeof(BooleanProperty),
                                  BOOLEAN_PROPERTIES_PER_PAGE)) == NULL) {
        return DOCUMENT_ERROR_MEMORY;
    }

    Property *newProperty = &(doc->props[doc->propsLen]);
    newProperty->nodeID = nodeID;
    newProperty->keyID = keyID;
    newProperty->valueID = valueID;
    doc->propsLen++;
    return DOCUMENT_SUCCESS;
}

DocumentStatus addTextNode(const node_id nodeID, const element_id textID,
                           Document *doc) {
    if ((doc->text = resizeArray(doc->text, doc->textLen, &doc->textCap,
                                 sizeof(TextNode), TEXT_NODES_PER_PAGE)) ==
        NULL) {
        return DOCUMENT_ERROR_MEMORY;
    }

    TextNode *newTextNode = &(doc->text[doc->textLen]);
    newTextNode->nodeID = nodeID;
    newTextNode->textID = textID;
    doc->textLen++;
    return DOCUMENT_SUCCESS;
}

void destroyDocument(const Document *doc) {
    free((void *)doc->nodes);
    free((void *)doc->parentFirstChilds);
    free((void *)doc->nextNodes);
    free((void *)doc->boolProps);
    free((void *)doc->props);
    free((void *)doc->text);
}
