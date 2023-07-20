#include <stdlib.h>

#include "dom/document.h"
#include "tokenizer/parse.h"
#include "utils/print/error.h"

DocumentStatus createDocument(const char *xmlString, Document *doc) {
    doc->nodes = malloc(NODES_PAGE_SIZE);
    doc->nodeLen = 0;
    doc->nodeCapacity = NODES_PER_PAGE;

    doc->parentFirstChilds = malloc(PARENT_CHILDS_PAGE_SIZE);
    doc->parentFirstChildLen = 0;
    doc->parentFirstChildCapacity = PARENT_CHILDS_PER_PAGE;

    doc->nextNodes = malloc(NEXT_NODES_PAGE_SIZE);
    doc->nextNodeLen = 0;
    doc->nextNodeCapacity = NEXT_NODES_PER_PAGE;

    doc->nodeAttributes = malloc(ATTRIBUTE_NODES_PAGE_SIZE);
    doc->nodeAttributeLen = 0;
    doc->nodeAttributeCapacity = ATTRIBUTE_NODES_PER_PAGE;

    doc->nodeAttributeValues = malloc(ATTRIBUTE_VALUE_NODES_PAGE_SIZE);
    doc->nodeAttributeValueLen = 0;
    doc->nodeAttributeValueCapacity = ATTRIBUTE_VALUE_NODES_PER_PAGE;

    if (doc->nodes == NULL || doc->parentFirstChilds == NULL ||
        doc->nextNodes == NULL || doc->nodeAttributes == NULL ||
        doc->nodeAttributeValues == NULL) {
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

void *resizeArray(void *array, size_t currentLen, size_t *totalCapacity,
                  size_t elementSize, size_t extraElements) {
    size_t newCapacity = (currentLen + extraElements) * elementSize;
    void *newArray = realloc(array, newCapacity);
    if (newArray == NULL) {
        PRINT_ERROR("Failed to reallocate memory for the array.\n");
        return NULL;
    }
    *totalCapacity = newCapacity / elementSize;
    return newArray;
}

DocumentStatus addNode(node_id *nodeID, element_id tagID, Document *doc) {
    if (doc->nodeLen >= doc->nodeCapacity) {
        doc->nodes = resizeArray(doc->nodes, doc->nodeLen, &doc->nodeCapacity,
                                 sizeof(Node), NODES_PER_PAGE);
        if (doc->nodes == NULL) {
            PRINT_ERROR("Failed to reallocate memory for nodes.\n");
            return DOCUMENT_ERROR_MEMORY;
        }
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
    if (doc->parentFirstChildLen >= doc->parentFirstChildCapacity) {
        doc->parentFirstChilds =
            resizeArray(doc->parentFirstChilds, doc->parentFirstChildLen,
                        &doc->parentFirstChildCapacity,
                        sizeof(ParentFirstChild), PARENT_CHILDS_PER_PAGE);
        if (doc->parentFirstChilds == NULL) {
            PRINT_ERROR(
                "Failed to reallocate memory for parent first child array.\n");
            return DOCUMENT_ERROR_MEMORY;
        }
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
    if (doc->nextNodeLen >= doc->nextNodeCapacity) {
        doc->nextNodes = resizeArray(doc->nextNodes, doc->nextNodeLen,
                                     &doc->nextNodeCapacity, sizeof(NextNode),
                                     NEXT_NODES_PER_PAGE);
        if (doc->nextNodes == NULL) {
            PRINT_ERROR("Failed to reallocate memory for next node array.\n");
            return DOCUMENT_ERROR_MEMORY;
        }
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

DocumentStatus addAttributeNode(const node_id nodeID,
                                const element_id attributeID, Document *doc) {
    if (doc->nodeAttributeLen >= doc->nodeAttributeCapacity) {
        doc->nodeAttributes = resizeArray(
            doc->nodeAttributes, doc->nodeAttributeLen,
            &doc->nodeAttributeCapacity, sizeof(NextNode), NEXT_NODES_PER_PAGE);
        if (doc->nodeAttributes == NULL) {
            PRINT_ERROR("Failed to reallocate memory for next node array.\n");
            return DOCUMENT_ERROR_MEMORY;
        }
    }

    NodeAttribute *newNodeAttribute =
        &(doc->nodeAttributes[doc->nodeAttributeLen]);
    newNodeAttribute->nodeID = nodeID;
    newNodeAttribute->attributeID = attributeID;
    doc->nodeAttributeLen++;
    return DOCUMENT_SUCCESS;
}

void destroyDocument(const Document *doc) {
    free((void *)doc->nodes);
    free((void *)doc->parentFirstChilds);
    free((void *)doc->nextNodes);
    free((void *)doc->nodeAttributes);
    free((void *)doc->nodeAttributeValues);
}
