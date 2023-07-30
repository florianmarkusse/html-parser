#include <stdio.h>
#include <string.h>

#include "dom/document-utils.h"
#include "dom/document.h"
#include "type/node/text-node.h"
#include "utils/file/path.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

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

    if (isText(node.tagID)) {
        const char *text = getText(node.nodeID, doc);
        if (text != NULL) {
            fprintf(output, "%s", text);
        }
        return;
    }

    const char *tag = gTags.container.elements[node.tagID];
    fprintf(output, "<%s", tag);

    for (size_t i = 0; i < doc->boolPropsLen; i++) {
        BooleanProperty boolProp = doc->boolProps[i];

        if (boolProp.nodeID == node.nodeID) {
            char *prop = gPropKeys.container.elements[boolProp.propID];
            fprintf(output, " %s", prop);
        }
    }

    for (size_t i = 0; i < doc->propsLen; i++) {
        Property prop = doc->props[i];

        if (prop.nodeID == node.nodeID) {
            char *key = gPropKeys.container.elements[prop.keyID];
            char *value = gPropValues.container.elements[prop.valueID];
            fprintf(output, " %s=\"%s\"", key, value);
        }
    }

    if (isSingle(node.tagID)) {
        if (strcmp(tag, "!DOCTYPE") == 0) {
            fprintf(output, ">");
        } else {
            fprintf(output, " />");
        }
        return;
    }
    fprintf(output, ">");
    node_id childNode = getFirstChild(nodeID, doc);
    while (childNode) {
        printNode(childNode, indentation + 1, doc, output);
        childNode = getNextNode(childNode, doc);
    }
    fprintf(output, "</%s>", tag);
}

void printHTML(const Document *doc) {
    printf("printing HTML...\n\n");
    node_id currentNodeID = doc->firstNodeID;
    while (currentNodeID) {
        printNode(currentNodeID, 0, doc, stdout);
        currentNodeID = getNextNode(currentNodeID, doc);
    }
    printf("\n\n");
}

FileStatus writeHTMLToFile(const Document *doc, const char *filePath) {
    createPath(filePath);
    FILE *file = fopen(filePath, "wbe");
    if (file == NULL) {
        printf("Failed to open file for writing: %s\n", filePath);
        return FILE_CANT_OPEN;
    }

    node_id currentNodeID = doc->firstNodeID;
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
        const char *type = gTags.container.elements[node.tagID];

        size_t bufferSize = sizeof(element_id) * 8 + 1;
        char bitBuffer[bufferSize];
        getBits(node.tagID, bitBuffer, bufferSize);
        printf("tag: %-6u bits: %-18s", node.tagID, bitBuffer);

        if (isSingle(node.tagID)) {
            printf("%-8s %-20s with node ID: %-4hu\n", "single",
                   isText(node.tagID) ? "--text--" : type, node.nodeID);
        } else {
            printf("%-8s %-20s with node ID: %-4hu\n", "paired", type,
                   node.nodeID);
        }
    }
    printf("\n");

    printf("text nodes inside document...\n");
    printf("total number of text nodes: %zu\n", doc->textLen);
    for (size_t i = 0; i < doc->textLen; i++) {
        TextNode textNode = doc->text[i];
        const char *type = gText.container.elements[textNode.textID];

        size_t bufferSize = sizeof(element_id) * 8 + 1;
        char bitBuffer[bufferSize];
        getBits(textNode.textID, bitBuffer, bufferSize);
        printf("text: %-6u bits: %-18s", textNode.textID, bitBuffer);
        printf("%-20s with node ID: %-4hu\n", type, textNode.nodeID);
    }
    printf("\n");

    printf("boolean property nodes inside document...\n");
    printf("total number of boolean properties: %zu\n", doc->boolPropsLen);
    for (size_t i = 0; i < doc->boolPropsLen; i++) {
        BooleanProperty boolProps = doc->boolProps[i];
        const char *type = gPropKeys.container.elements[boolProps.propID];

        size_t bufferSize = sizeof(element_id) * 8 + 1;
        char bitBuffer[bufferSize];
        getBits(boolProps.propID, bitBuffer, bufferSize);
        printf("boolean property: %-6u bits: %-18s", boolProps.propID,
               bitBuffer);

        printf("%-20s with node ID: %-4hu\n", type, boolProps.nodeID);
    }
    printf("\n");

    printf("key-value property nodes inside document...\n");
    printf("total number of key-value properties: %zu\n", doc->propsLen);
    for (size_t i = 0; i < doc->propsLen; i++) {
        Property property = doc->props[i];
        const char *key = gPropKeys.container.elements[property.keyID];
        const char *value = gPropValues.container.elements[property.valueID];

        size_t bufferSize = sizeof(element_id) * 8 + 1;
        char bitBuffer[bufferSize];
        getBits(property.keyID, bitBuffer, bufferSize);
        printf("key: %-6u bits: %-18s", property.keyID, bitBuffer);
        getBits(property.valueID, bitBuffer, bufferSize);
        printf("value: %-6u bits: %-18s", property.valueID, bitBuffer);

        printf("%-20s %-20s with node ID: %-4hu\n", key, value,
               property.nodeID);
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

    printf("parent-child inside document...\n");
    printf("total number of parent-child: %zu\n", doc->parentChildLen);
    for (size_t i = 0; i < doc->parentChildLen; i++) {
        printf("parent: %-4hu child: %-4hu\n", doc->parentChilds[i].parentID,
               doc->parentChilds[i].childID);
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
