#include <stdio.h>
#include <string.h>

#include "flo/html-parser/dom/dom-utils.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/type/node/text-node.h"
#include "flo/html-parser/utils/file/path.h"

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
    bits[numBits] = '\0';
}

void printNode(const node_id nodeID, const size_t indentation, const Dom *dom,
               const DataContainer *dataContainer, FILE *output) {
    if (nodeID == ERROR_NODE_ID) {
        return;
    }
    Node node = dom->nodes[nodeID];

    if (isText(node.tagID)) {
        const char *text = getText(node.nodeID, dom, dataContainer);
        if (text != NULL) {
            fprintf(output, "%s", text);
        }
        return;
    }

    const char *tag = dataContainer->tags.container.elements[node.tagID];
    fprintf(output, "<%s", tag);

    for (size_t i = 0; i < dom->boolPropsLen; i++) {
        BooleanProperty boolProp = dom->boolProps[i];

        if (boolProp.nodeID == node.nodeID) {
            char *prop =
                dataContainer->propKeys.container.elements[boolProp.propID];
            fprintf(output, " %s", prop);
        }
    }

    for (size_t i = 0; i < dom->propsLen; i++) {
        Property prop = dom->props[i];

        if (prop.nodeID == node.nodeID) {
            char *key = dataContainer->propKeys.container.elements[prop.keyID];
            char *value =
                dataContainer->propValues.container.elements[prop.valueID];
            fprintf(output, " %s=\"%s\"", key, value);
        }
    }

    if (isSingle(node.tagID)) {
        if (strcmp(tag, "!domTYPE") == 0) {
            fprintf(output, ">");
        } else {
            fprintf(output, "/>");
        }
        return;
    }
    fprintf(output, ">");
    node_id childNode = getFirstChild(nodeID, dom);
    while (childNode) {
        printNode(childNode, indentation + 1, dom, dataContainer, output);
        childNode = getNextNode(childNode, dom);
    }
    fprintf(output, "</%s>", tag);
}

void printHTML(const Dom *dom, const DataContainer *dataContainer) {
    printf("printing HTML...\n\n");
    node_id currentNodeID = dom->firstNodeID;
    while (currentNodeID) {
        printNode(currentNodeID, 0, dom, dataContainer, stdout);
        currentNodeID = getNextNode(currentNodeID, dom);
    }
    printf("\n\n");
}

FileStatus writeHTMLToFile(const Dom *dom, const DataContainer *dataContainer,
                           const char *filePath) {
    createPath(filePath);
    FILE *file = fopen(filePath, "wbe");
    if (file == NULL) {
        printf("Failed to open file for writing: %s\n", filePath);
        return FILE_CANT_OPEN;
    }

    node_id currentNodeID = dom->firstNodeID;
    while (currentNodeID) {
        printNode(currentNodeID, 0, dom, dataContainer, file);
        currentNodeID = getNextNode(currentNodeID, dom);
    }

    fclose(file);

    return FILE_SUCCESS;
}

void printdomumentStatus(const Dom *dom, const DataContainer *dataContainer) {
    printf("printing domument status...\n\n");
    printf(" in here\n");
    printf("single lenn %u\n", dataContainer->tags.singleLen);
    printf("single offset %u\n", SINGLES_OFFSET);
    printf("paired len %u\n", dataContainer->tags.pairedLen);

    printf("nodes inside domument...\n");
    printf("total number of nodes: %zu\n", dom->nodeLen);
    for (size_t i = 0; i < dom->nodeLen; i++) {
        Node node = dom->nodes[i];

        if (node.nodeID == 0) {
            printf("tag: %-6s bits: %-18s", "xxxxx", "xxxxxxxxxxxxxxxx");
            printf("%-8s %-20s with node ID: %-4hu\n", "error", "internal use",
                   node.nodeID);
        } else {
            const char *type =
                dataContainer->tags.container.elements[node.tagID];

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
    }
    printf("\n");

    printf("text nodes inside dom...\n");
    printf("total number of text nodes: %zu\n", dom->textLen);
    for (size_t i = 0; i < dom->textLen; i++) {
        TextNode textNode = dom->text[i];
        const char *type =
            dataContainer->text.container.elements[textNode.textID];

        size_t bufferSize = sizeof(element_id) * 8 + 1;
        char bitBuffer[bufferSize];
        getBits(textNode.textID, bitBuffer, bufferSize);
        printf("text: %-6u bits: %-18s", textNode.textID, bitBuffer);
        printf("%-20s with node ID: %-4hu\n", type, textNode.nodeID);
    }
    printf("\n");

    printf("boolean property nodes inside dom...\n");
    printf("total number of boolean properties: %zu\n", dom->boolPropsLen);
    for (size_t i = 0; i < dom->boolPropsLen; i++) {
        BooleanProperty boolProps = dom->boolProps[i];
        const char *type =
            dataContainer->propKeys.container.elements[boolProps.propID];

        size_t bufferSize = sizeof(element_id) * 8 + 1;
        char bitBuffer[bufferSize];
        getBits(boolProps.propID, bitBuffer, bufferSize);
        printf("boolean property: %-6u bits: %-18s", boolProps.propID,
               bitBuffer);

        printf("%-20s with node ID: %-4hu\n", type, boolProps.nodeID);
    }
    printf("\n");

    printf("key-value property nodes inside dom...\n");
    printf("total number of key-value properties: %zu\n", dom->propsLen);
    for (size_t i = 0; i < dom->propsLen; i++) {
        Property property = dom->props[i];
        const char *key =
            dataContainer->propKeys.container.elements[property.keyID];
        const char *value =
            dataContainer->propValues.container.elements[property.valueID];

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

    printf("parent-first-child inside dom...\n");
    printf("total number of parent-first-child: %zu\n",
           dom->parentFirstChildLen);
    for (size_t i = 0; i < dom->parentFirstChildLen; i++) {
        printf("parent: %-4hu first child: %-4hu\n",
               dom->parentFirstChilds[i].parentID,
               dom->parentFirstChilds[i].childID);
    }
    printf("\n");

    printf("parent-child inside dom...\n");
    printf("total number of parent-child: %zu\n", dom->parentChildLen);
    for (size_t i = 0; i < dom->parentChildLen; i++) {
        printf("parent: %-4hu child: %-4hu\n", dom->parentChilds[i].parentID,
               dom->parentChilds[i].childID);
    }
    printf("\n");

    printf("next nodes inside dom...\n");
    printf("total number of next nodes: %zu\n", dom->nextNodeLen);
    for (size_t i = 0; i < dom->nextNodeLen; i++) {
        printf("current node: %-4hu next node: %-4hu\n",
               dom->nextNodes[i].currentNodeID, dom->nextNodes[i].nextNodeID);
    }
    printf("\n\n");
}
