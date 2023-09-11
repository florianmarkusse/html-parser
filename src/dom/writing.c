#include <stdio.h>
#include <string.h>

#include "flo/html-parser/dom/dom-utils.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/type/node/text-node.h"
#include "flo/html-parser/utils/file/path.h"

void printNode(const node_id nodeID, const size_t indentation, const Dom *dom,
               const DataContainer *dataContainer, FILE *output) {
    Node node = dom->nodes[nodeID];

    if (node.nodeType == NODE_TYPE_ERROR) {
        return;
    }

    if (node.nodeType == NODE_TYPE_TEXT) {
        fprintf(output, "%s", node.text);
        return;
    }

    const char *tag = getTag(node.tagID, dom, dataContainer);
    fprintf(output, "<%s", tag);

    for (size_t i = 0; i < dom->boolPropsLen; i++) {
        BooleanProperty boolProp = dom->boolProps[i];

        if (boolProp.nodeID == node.nodeID) {
            const char *prop = getBoolProp(boolProp.propID, dom, dataContainer);
            fprintf(output, " %s", prop);
        }
    }

    for (size_t i = 0; i < dom->propsLen; i++) {
        Property prop = dom->props[i];

        if (prop.nodeID == node.nodeID) {
            const char *key = getPropKey(prop.keyID, dom, dataContainer);
            const char *value = getPropValue(prop.valueID, dom, dataContainer);
            fprintf(output, " %s=\"%s\"", key, value);
        }
    }

    TagRegistration *tagRegistration = NULL;
    getTagRegistration(node.tagID, dom, &tagRegistration);
    if (!tagRegistration->isPaired) {
        if (strcmp(tag, "!DOCTYPE") == 0) {
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
        childNode = getNext(childNode, dom);
    }
    fprintf(output, "</%s>", tag);
}

void printHTML(const Dom *dom, const DataContainer *dataContainer) {
    printf("printing HTML...\n\n");
    node_id currentNodeID = dom->firstNodeID;
    while (currentNodeID) {
        printNode(currentNodeID, 0, dom, dataContainer, stdout);
        currentNodeID = getNext(currentNodeID, dom);
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
        currentNodeID = getNext(currentNodeID, dom);
    }

    fclose(file);

    return FILE_SUCCESS;
}

void printBasicRegistry(const char *registryName,
                        const BasicRegistry *basicRegistry,
                        const StringHashSet *set) {
    printf("%-20s registration nodes inside DOM...\n", registryName);
    printf("total number of nodes: %zu\n", basicRegistry->len);
    for (size_t i = 0; i < basicRegistry->len; i++) {
        Registration registration = basicRegistry->registry[i];
        const char *value =
            getStringFromHashSet(set, &registration.hashElement);
        printf("index ID: %-5u value: %-20s hash: %zu offset: %u\n",
               registration.indexID, value, registration.hashElement.hash,
               registration.hashElement.offset);
    }
    printf("\n");
}

void printDomStatus(const Dom *dom, const DataContainer *dataContainer) {
    printf("printing DOM status...\n\n");

    printf("nodes inside DOM...\n");
    printf("total number of nodes: %zu\n", dom->nodeLen);
    for (size_t i = 0; i < dom->nodeLen; i++) {
        Node node = dom->nodes[i];

        if (node.nodeType == NODE_TYPE_TEXT) {
            printf("node ID: %-5u node type: %-10s containing text\n",
                   node.nodeID, nodeTypeToString(node.nodeType));
        } else {
            printf("node ID: %-5u node type: %-10s with tag ID: %-5u\n",
                   node.nodeID, nodeTypeToString(node.nodeType), node.tagID);
        }
    }
    printf("\n");

    printf("tag registration nodes inside DOM...\n");
    printf("total number of tag registration nodes: %zu\n",
           dom->tagRegistryLen);
    for (size_t i = 0; i < dom->tagRegistryLen; i++) {
        TagRegistration tagRegistration = dom->tagRegistry[i];
        const char *tag = getStringFromHashSet(&dataContainer->tags.set,
                                               &tagRegistration.hashElement);
        printf("tag ID: %-5u tag: %-20s isPaired: %d hash: %zu offset: %u\n",
               tagRegistration.tagID, tag, tagRegistration.isPaired,
               tagRegistration.hashElement.hash,
               tagRegistration.hashElement.offset);
    }
    printf("\n");

    printBasicRegistry("bool props", &dom->boolPropRegistry,
                       &dataContainer->boolProps.set);
    printBasicRegistry("key props", &dom->propKeyRegistry,
                       &dataContainer->propKeys.set);
    printBasicRegistry("value props", &dom->propValueRegistry,
                       &dataContainer->propValues.set);

    printf("boolean property nodes inside DOM...\n");
    printf("total number of boolean properties: %zu\n", dom->boolPropsLen);
    for (size_t i = 0; i < dom->boolPropsLen; i++) {
        BooleanProperty boolProps = dom->boolProps[i];
        printf("node ID: %-5u prop ID: %-5u\n", boolProps.nodeID,
               boolProps.propID);
    }
    printf("\n");

    printf("key-value property nodes inside DOM...\n");
    printf("total number of key-value properties: %zu\n", dom->propsLen);
    for (size_t i = 0; i < dom->propsLen; i++) {
        Property property = dom->props[i];
        printf("node ID: %-5u key ID: %-5u value ID: %-5u\n", property.nodeID,
               property.keyID, property.valueID);
    }
    printf("\n");

    printf("parent-first-child inside DOM...\n");
    printf("total number of parent-first-child: %zu\n",
           dom->parentFirstChildLen);
    for (size_t i = 0; i < dom->parentFirstChildLen; i++) {
        printf("parent node ID: %-5u first child node ID: %-5u\n",
               dom->parentFirstChilds[i].parentID,
               dom->parentFirstChilds[i].childID);
    }
    printf("\n");

    printf("parent-child inside DOM...\n");
    printf("total number of parent-child: %zu\n", dom->parentChildLen);
    for (size_t i = 0; i < dom->parentChildLen; i++) {
        printf("parent: %-5u child: %-5u\n", dom->parentChilds[i].parentID,
               dom->parentChilds[i].childID);
    }
    printf("\n");

    printf("next nodes inside DOM...\n");
    printf("total number of next nodes: %zu\n", dom->nextNodeLen);
    for (size_t i = 0; i < dom->nextNodeLen; i++) {
        printf("current node: %-5u next node: %-5u\n",
               dom->nextNodes[i].currentNodeID, dom->nextNodes[i].nextNodeID);
    }
    printf("\n\n");
}
