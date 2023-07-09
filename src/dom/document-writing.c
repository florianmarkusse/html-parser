#include <stdio.h>

#include "dom/document.h"
#include "utils/file/path.h"

void printBits(const element_id tagID) {
    unsigned char numBits = sizeof(element_id) * 8;
    for (unsigned char i = 0; i < numBits; i++) {
        unsigned char bit = (tagID >> (numBits - 1 - i)) & 1;
        printf("%hhu", bit);
    }
}

void getBits(const element_id tagID, char *bits, const size_t size) {
    unsigned char numBits = sizeof(element_id) * 8;
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
               const Document *doc, FILE *output) {
    Node node = doc->nodes[nodeID - 1];
    for (size_t i = 0; i < indentation; i++) {
        fprintf(output, "  ");
    }

    const char *tag = globalTags.elements[node.tagID];
    fprintf(output, "<%s", tag);

    for (size_t i = 0; i < doc->nodeAttributeLen; i++) {
        NodeAttribute nodeAttribute = doc->nodeAttributes[i];

        if (nodeAttribute.nodeID == node.nodeID) {
            char *attribute =
                globalAttributes.elements[nodeAttribute.attributeID];
            if (isSingle(nodeAttribute.attributeID)) {
                fprintf(output, " %s", attribute);
            } else {
                fprintf(output, " %s=%s", attribute, "NOT IMPLEMENTED");
            }
        }
    }

    if (isSingle(node.tagID)) {
        fprintf(output, "/>\n");
        return;
    }
    fprintf(output, ">\n");
    node_id childNode = getFirstChild(nodeID, doc);
    while (childNode) {
        printNode(childNode, indentation + 1, doc, output);
        childNode = getNextNode(childNode, doc);
    }
    for (size_t i = 0; i < indentation; i++) {
        fprintf(output, "  ");
    }
    fprintf(output, "</%s>\n", tag);
}

void printXML(const Document *doc) {
    printf("printing XML...\n\n");
    node_id currentNodeID = 1;
    while (currentNodeID) {
        printNode(currentNodeID, 0, doc, stdout);
        currentNodeID = getNextNode(currentNodeID, doc);
    }
    printf("\n\n");
}

FileStatus writeXMLToFile(const Document *doc, const char *filePath) {
    createPath(filePath);
    FILE *file = fopen(filePath, "wbe");
    if (file == NULL) {
        printf("Failed to open file for writing: %s\n", filePath);
        return FILE_CANT_OPEN;
    }

    node_id currentNodeID = 1;
    while (currentNodeID) {
        printNode(currentNodeID, 0, doc, file);
        currentNodeID = getNextNode(currentNodeID, doc);
    }

    fclose(file);

    return FILE_SUCCESS;
}

void printDocumentStatus(const Document *doc) {
    printf("printing document status...\n\n");

    printf("nodes inside document...\n");
    printf("total number of nodes: %zu\n", doc->nodeLen);
    for (size_t i = 0; i < doc->nodeLen; i++) {
        Node node = doc->nodes[i];
        const char *type = globalTags.elements[node.tagID];

        size_t bufferSize = sizeof(element_id) * 8 + 1;
        char bitBuffer[bufferSize];
        getBits(node.tagID, bitBuffer, bufferSize);
        printf("tag: %-4u bits: %-18s", node.tagID, bitBuffer);

        if (isSingle(node.tagID)) {
            printf("%-8s %-20s with node ID: %-4hu\n", "single", type,
                   node.nodeID);
        } else {
            printf("%-8s %-20s with node ID: %-4hu\n", "paired", type,
                   node.nodeID);
        }
    }
    printf("\n");

    printf("attribute nodes inside document...\n");
    printf("total number of attribute nodes: %zu\n", doc->nodeAttributeLen);
    for (size_t i = 0; i < doc->nodeAttributeLen; i++) {
        NodeAttribute nodeAttribute = doc->nodeAttributes[i];
        const char *type = globalAttributes.elements[nodeAttribute.attributeID];

        size_t bufferSize = sizeof(element_id) * 8 + 1;
        char bitBuffer[bufferSize];
        getBits(nodeAttribute.attributeID, bitBuffer, bufferSize);
        printf("attribute: %-4u bits: %-18s", nodeAttribute.attributeID,
               bitBuffer);

        if (isSingle(nodeAttribute.attributeID)) {
            printf("%-8s %-20s with node ID: %-4hu\n", "single", type,
                   nodeAttribute.nodeID);
        } else {
            printf("%-8s %-20s with node ID: %-4hu\n", "paired", type,
                   nodeAttribute.nodeID);
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
