#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "dom/document.h"
#include "tokenizer/parse.h"
#include "type/data-page.h"
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

    if (doc->nodes == NULL || doc->parentFirstChilds == NULL ||
        doc->nextNodes == NULL) {
        PRINT_ERROR("Failed to allocate memory for nodes.\n");
        destroyDocument(doc);
        return DOCUMENT_ERROR_MEMORY;
    }

    DocumentStatus documentStatus = parse(xmlString, doc);
    if (documentStatus != DOCUMENT_SUCCESS) {
        ERROR_WITH_CODE_ONLY(documentStatusToString(documentStatus),
                             "Failed to parse document");
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

DocumentStatus addNode(node_id *nodeID, tag_id tagID, Document *doc) {
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
        doc->nodeLen +
        1; // We start at 1 because the parse variables are initialized to 0.
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

void destroyDocument(const Document *doc) {
    free((void *)doc->nodes);
    free((void *)doc->parentFirstChilds);
    free((void *)doc->nextNodes);
}

void printBits(const tag_id tagID) {
    unsigned char numBits = sizeof(tag_id) * 8;
    for (unsigned char i = 0; i < numBits; i++) {
        unsigned char bit = (tagID >> (numBits - 1 - i)) & 1;
        printf("%hhu", bit);
    }
}

void getBits(const tag_id tagID, char *bits, const size_t size) {
    unsigned char numBits = sizeof(tag_id) * 8;
    if (size < numBits + 1) {
        fprintf(stderr, "Insufficient buffer size in getBits\n");
        return;
    }

    for (unsigned char i = 0; i < numBits; i++) {
        unsigned char bit = (tagID >> (numBits - 1 - i)) & 1;
        bits[i] = '0' + bit;
    }
    bits[numBits] = '\0'; // Add null terminator
}

void printNode(const node_id nodeID, const size_t indentation,
               const Document *doc) {
    Node node = doc->nodes[nodeID - 1];
    for (size_t i = 0; i < indentation; i++) {
        printf("  ");
    }

    const char *tag = globalTags.tags[node.tagID];
    if (isSelfClosing(node.tagID)) {
        printf("<%s />\n", tag);
        return;
    }
    printf("<%s>\n", tag);
    node_id childNode = getFirstChild(nodeID, doc);
    while (childNode) {
        printNode(childNode, indentation + 1, doc);
        childNode = getNextNode(childNode, doc);
    }
    for (size_t i = 0; i < indentation; i++) {
        printf("  ");
    }
    printf("</%s>\n", tag);
}

void printXML(const Document *doc) {
    printf("printing XML...\n\n");
    node_id currentNodeID = 1;
    while (currentNodeID) {
        printNode(currentNodeID, 0, doc);
        currentNodeID = getNextNode(currentNodeID, doc);
    }
    printf("\n\n");
}

void printDocumentStatus(const Document *doc) {
    printf("printing document status...\n\n");

    printf("nodes inside document...\n");
    printf("total number of nodes: %zu\n", doc->nodeLen);
    for (size_t i = 0; i < doc->nodeLen; i++) {
        Node node = doc->nodes[i];
        const char *type = globalTags.tags[node.tagID];

        size_t bufferSize = sizeof(tag_id) * 8 + 1;
        char bitBuffer[bufferSize];
        getBits(node.tagID, bitBuffer, bufferSize);
        printf("tag: %-4u bits: %-18s", node.tagID, bitBuffer);

        if (isSelfClosing(node.tagID)) {
            printf("%-8s %-20s with node ID: %-4hu\n", "single", type,
                   node.nodeID);
        } else {
            printf("%-8s %-20s with node ID: %-4hu\n", "paired", type,
                   node.nodeID);
        }
    }
    printf("\n");

    printf("parent-first-child inside document...\n");
    printf("total number of parent-first-child: %zu\n",
           doc->parentFirstChildLen);
    for (size_t i = 0; i < doc->parentFirstChildLen; i++) {
        printf("parent: %-4hu first child: %-4hu\n",
               doc->parentFirstChilds[i].parentID,
               doc->parentFirstChilds[i].childID);
    }
    printf("\n");

    printf("next nodes inside document...\n");
    printf("total number of next nodes: %zu\n", doc->nextNodeLen);
    for (size_t i = 0; i < doc->nextNodeLen; i++) {
        printf("current node: %-4hu next node: %-4hu\n",
               doc->nextNodes[i].currentNodeID, doc->nextNodes[i].nextNodeID);
    }

    printf("\n\n");
}
