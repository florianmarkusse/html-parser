#include <stdio.h>
#include <string.h>

#include "flo/html-parser/dom/dom-utils.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/type/element/elements-print.h"
#include "flo/html-parser/type/node/text-node.h"
#include "flo/html-parser/utils/file/path.h"

void printNode(const flo_html_node_id nodeID, const size_t indentation,
               const flo_html_Dom *dom, const flo_html_TextStore *textStore,
               FILE *output) {
    flo_html_Node node = dom->nodes[nodeID];

    if (node.nodeType == NODE_TYPE_ERROR) {
        return;
    }

    if (node.nodeType == NODE_TYPE_TEXT) {
        fprintf(output, "%s", node.text.buf);
        return;
    }

    const flo_html_String tag = flo_html_getTag(node.tagID, dom, textStore);
    fprintf(output, "<%s", tag.buf);

    for (size_t i = 0; i < dom->boolPropsLen; i++) {
        flo_html_BooleanProperty boolProp = dom->boolProps[i];

        if (boolProp.nodeID == node.nodeID) {
            const flo_html_String prop =
                flo_html_getBoolProp(boolProp.propID, dom, textStore);
            fprintf(output, " %s", prop.buf);
        }
    }

    for (size_t i = 0; i < dom->propsLen; i++) {
        flo_html_Property prop = dom->props[i];

        if (prop.nodeID == node.nodeID) {
            const flo_html_String key =
                flo_html_getPropKey(prop.keyID, dom, textStore);
            const flo_html_String value =
                flo_html_getPropValue(prop.valueID, dom, textStore);
            fprintf(output, " %s=\"%s\"", key.buf, value.buf);
        }
    }

    flo_html_TagRegistration *tagRegistration = NULL;
    flo_html_getTagRegistration(node.tagID, dom, &tagRegistration);
    if (!tagRegistration->isPaired) {
        if (flo_html_stringEquals(tag, FLO_HTML_S("!DOCTYPE"))) {
            fprintf(output, ">");
        } else {
            fprintf(output, "/>");
        }
        return;
    }
    fprintf(output, ">");
    flo_html_node_id childNode = flo_html_getFirstChild(nodeID, dom);
    while (childNode) {
        printNode(childNode, indentation + 1, dom, textStore, output);
        childNode = flo_html_getNext(childNode, dom);
    }
    fprintf(output, "</%s>", tag.buf);
}

void flo_html_printHTML(const flo_html_Dom *dom,
                        const flo_html_TextStore *textStore) {
    printf("printing HTML...\n\n");
    flo_html_node_id currentNodeID = dom->firstNodeID;
    while (currentNodeID) {
        printNode(currentNodeID, 0, dom, textStore, stdout);
        currentNodeID = flo_html_getNext(currentNodeID, dom);
    }
    printf("\n\n");
}

flo_html_FileStatus
flo_html_writeHTMLToFile(const flo_html_Dom *dom,
                         const flo_html_TextStore *textStore,
                         const flo_html_String filePath) {
    flo_html_createPath(filePath.buf);
    FILE *file = fopen(filePath.buf, "wbe");
    if (file == NULL) {
        printf("Failed to open file for writing: %s\n", filePath.buf);
        return FILE_CANT_OPEN;
    }

    flo_html_node_id currentNodeID = dom->firstNodeID;
    while (currentNodeID) {
        printNode(currentNodeID, 0, dom, textStore, file);
        currentNodeID = flo_html_getNext(currentNodeID, dom);
    }

    fclose(file);

    return FILE_SUCCESS;
}

void printflo_html_BasicRegistry(const flo_html_String registryName,
                                 const flo_html_BasicRegistry *basicRegistry,
                                 const flo_html_StringHashSet *set) {
    printf("%-20s registration nodes inside DOM...\n", registryName.buf);
    printf("total number of nodes: %zu\n", basicRegistry->len);
    for (size_t i = 0; i < basicRegistry->len; i++) {
        flo_html_Registration registration = basicRegistry->registry[i];
        const flo_html_String value =
            flo_html_getStringFromHashSet(set, &registration.hashElement);
        printf("index ID: %-5u value: %-20s hash: %zu offset: %u\n",
               registration.flo_html_indexID, value.buf,
               registration.hashElement.hash, registration.hashElement.offset);
    }
    printf("\n");
}

void flo_html_printDomStatus(const flo_html_Dom *dom,
                             const flo_html_TextStore *textStore) {
    printf("printing DOM status...\n\n");

    printf("Printing Text Store contents...\n");
    flo_html_printPropertyStatus(textStore);
    flo_html_printBoolPropStatus(textStore);
    flo_html_printTextStatus(textStore);
    flo_html_printTagStatus(textStore);

    printf("Printing DOM contents...\n");

    printf("nodes inside DOM...\n");
    printf("total number of nodes: %zu\n", dom->nodeLen);
    for (size_t i = 0; i < dom->nodeLen; i++) {
        flo_html_Node node = dom->nodes[i];

        if (node.nodeType == NODE_TYPE_TEXT) {
            printf("node ID: %-5u node type: %-10s containing text\n",
                   node.nodeID, flo_html_nodeTypeToString(node.nodeType));
        } else {
            printf("node ID: %-5u node type: %-10s with tag ID: %-5u\n",
                   node.nodeID, flo_html_nodeTypeToString(node.nodeType),
                   node.tagID);
        }
    }
    printf("\n");

    printf("tag registration nodes inside DOM...\n");
    printf("total number of tag registration nodes: %zu\n",
           dom->tagRegistryLen);
    for (size_t i = 0; i < dom->tagRegistryLen; i++) {
        flo_html_TagRegistration tagRegistration = dom->tagRegistry[i];
        const flo_html_String tag = flo_html_getStringFromHashSet(
            &textStore->tags.set, &tagRegistration.hashElement);
        printf("tag ID: %-5u tag: %-20s isPaired: %d hash: %zu offset: %u\n",
               tagRegistration.tagID, tag.buf, tagRegistration.isPaired,
               tagRegistration.hashElement.hash,
               tagRegistration.hashElement.offset);
    }
    printf("\n");

    printflo_html_BasicRegistry(FLO_HTML_S("bool props"),
                                &dom->boolPropRegistry,
                                &textStore->boolProps.set);
    printflo_html_BasicRegistry(FLO_HTML_S("key props"), &dom->propKeyRegistry,
                                &textStore->propKeys.set);
    printflo_html_BasicRegistry(FLO_HTML_S("value props"),
                                &dom->propValueRegistry,
                                &textStore->propValues.set);

    printf("boolean property nodes inside DOM...\n");
    printf("total number of boolean properties: %zu\n", dom->boolPropsLen);
    for (size_t i = 0; i < dom->boolPropsLen; i++) {
        flo_html_BooleanProperty boolProps = dom->boolProps[i];
        printf("node ID: %-5u prop ID: %-5u\n", boolProps.nodeID,
               boolProps.propID);
    }
    printf("\n");

    printf("key-value property nodes inside DOM...\n");
    printf("total number of key-value properties: %zu\n", dom->propsLen);
    for (size_t i = 0; i < dom->propsLen; i++) {
        flo_html_Property property = dom->props[i];
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
