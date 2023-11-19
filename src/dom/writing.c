#include <stdio.h>
#include <string.h>

#include "file/path.h"
#include "flo/html-parser/dom/dom-util.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/node/text-node.h"

void printNode(flo_html_node_id nodeID, ptrdiff_t indentation,
               flo_html_Dom *dom, FILE *output) {
    flo_html_Node node = dom->nodes.buf[nodeID];

    if (node.nodeType == NODE_TYPE_ERROR) {
        return;
    }

    if (node.nodeType == NODE_TYPE_ROOT) {
        flo_html_node_id childNode = flo_html_getFirstChild(nodeID, dom);
        while (childNode) {
            printNode(childNode, indentation + 1, dom, output);
            childNode = flo_html_getNext(childNode, dom);
        }
        return;
    }

    if (node.nodeType == NODE_TYPE_TEXT) {
        fprintf(output, "%.*s", FLO_STRING_PRINT(node.text));
        return;
    }

    flo_String tag = dom->tagRegistry.buf[node.tagID].tag;
    fprintf(output, "<%.*s", FLO_STRING_PRINT(tag));

    for (ptrdiff_t i = 0; i < dom->boolProps.len; i++) {
        flo_html_BooleanProperty boolProp = dom->boolProps.buf[i];

        if (boolProp.nodeID == node.nodeID) {
            fprintf(
                output, " %.*s",
                FLO_STRING_PRINT(dom->boolPropRegistry.buf[boolProp.propID]));
        }
    }

    for (ptrdiff_t i = 0; i < dom->props.len; i++) {
        flo_html_Property prop = dom->props.buf[i];

        if (prop.nodeID == node.nodeID) {
            fprintf(output, " %.*s=\"%.*s\"",
                    FLO_STRING_PRINT(dom->propKeyRegistry.buf[prop.keyID]),
                    FLO_STRING_PRINT(dom->propValueRegistry.buf[prop.valueID]));
        }
    }

    flo_html_TagRegistration *tagRegistration =
        &dom->tagRegistry.buf[node.tagID];
    if (!tagRegistration->isPaired) {
        if (flo_stringEquals(tag, FLO_STRING("!DOCTYPE"))) {
            fprintf(output, ">");
        } else {
            fprintf(output, "/>");
        }
        return;
    }
    fprintf(output, ">");

    flo_html_node_id childNode = flo_html_getFirstChild(nodeID, dom);
    while (childNode) {
        printNode(childNode, indentation + 1, dom, output);
        childNode = flo_html_getNext(childNode, dom);
    }

    fprintf(output, "</%.*s>", FLO_STRING_PRINT(tag));
}

void flo_html_printHTML(flo_html_Dom *dom) {
    printf("printing HTML...\n\n");
    flo_html_node_id currentNodeID = FLO_HTML_ROOT_NODE_ID;
    while (currentNodeID) {
        printNode(currentNodeID, 0, dom, stdout);
        currentNodeID = flo_html_getNext(currentNodeID, dom);
    }
    printf("\n\n");
}

flo_FileStatus flo_html_writeHTMLToFile(flo_html_Dom *dom, flo_String filePath,
                                        flo_Arena scratch) {
    flo_createPath(filePath, scratch);
    // casting here because filePath should not contain any funny characters.
    FILE *file = fopen((char *)filePath.buf, "wbe");
    if (file == NULL) {
        printf("Failed to open file for writing: %.*s\n",
               FLO_STRING_PRINT(filePath));
        return FILE_CANT_OPEN;
    }

    flo_html_node_id currentNodeID = FLO_HTML_ROOT_NODE_ID;
    while (currentNodeID) {
        printNode(currentNodeID, 0, dom, file);
        currentNodeID = flo_html_getNext(currentNodeID, dom);
    }

    fclose(file);

    return FILE_SUCCESS;
}

void printflo_html_BasicRegistry(flo_String registryName,
                                 flo_String_d_a *strings) {
    printf("%-20.*s\nregistration nodes inside DOM...\n",
           FLO_STRING_PRINT(registryName));
    printf("total number of nodes: %zu\n", strings->len);
    for (ptrdiff_t i = 0; i < strings->len; i++) {
        flo_String string = strings->buf[i];
        printf("ID: %zu value: %-20.*s\n", i, FLO_STRING_PRINT(string));
    }
    printf("\n");
}

void flo_html_printDomStatus(flo_html_Dom *dom, flo_Arena scratch) {
    printf("printing DOM status...\n\n");

    flo_trie_StringAutoUint16Data data;

    printf("printing property status...\n\n");
    printf("printing keys...\n");
    FLO_FOR_EACH_TRIE_STRING_AUTO_UINT16(data, dom->propKeyMap, &scratch) {
        printf("string = %.*s with id = %d\n", FLO_STRING_PRINT(data.key),
               data.value);
    }

    printf("printing values...\n");
    FLO_FOR_EACH_TRIE_STRING_AUTO_UINT16(data, dom->propValueMap, &scratch) {
        printf("string = %.*s with id = %d\n", FLO_STRING_PRINT(data.key),
               data.value);
    }

    printf("printing bool property status...\n\n");
    FLO_FOR_EACH_TRIE_STRING_AUTO_UINT16(data, dom->boolPropMap, &scratch) {
        printf("string = %.*s with id = %d\n", FLO_STRING_PRINT(data.key),
               data.value);
    }

    printf("printing tags status...\n\n");
    FLO_FOR_EACH_TRIE_STRING_AUTO_UINT16(data, dom->tagMap, &scratch) {
        printf("string = %.*s with id = %d\n", FLO_STRING_PRINT(data.key),
               data.value);
    }

    printf("Printing DOM contents...\n");

    printf("nodes inside DOM...\n");
    printf("total number of nodes: %zu\n", dom->nodes.len);
    for (ptrdiff_t i = 0; i < dom->nodes.len; i++) {
        flo_html_Node node = dom->nodes.buf[i];

        if (node.nodeType == NODE_TYPE_DOCUMENT) {
            printf("node ID: %-5u node type: %-10.*s with tag ID: %-5u\n",
                   node.nodeID,
                   FLO_STRING_PRINT(flo_html_nodeTypeToString(node.nodeType)),
                   node.tagID);
        } else {
            printf("node ID: %-5u node type: %-10.*s\n", node.nodeID,
                   FLO_STRING_PRINT(flo_html_nodeTypeToString(node.nodeType)));
        }
    }
    printf("\n");

    printf("tag registration nodes inside DOM...\n");
    printf("total number of tag registration nodes: %zu\n",
           dom->tagRegistry.len);
    for (ptrdiff_t i = 0; i < dom->tagRegistry.len; i++) {
        flo_html_TagRegistration tagRegistration = dom->tagRegistry.buf[i];
        printf("tag ID: %-5td tag: %-20.*s isPaired: %d\n", i,
               FLO_STRING_PRINT(tagRegistration.tag), tagRegistration.isPaired);
    }
    printf("\n");

    printflo_html_BasicRegistry(FLO_STRING("bool props"),
                                &dom->boolPropRegistry);
    printflo_html_BasicRegistry(FLO_STRING("key props"), &dom->propKeyRegistry);
    printflo_html_BasicRegistry(FLO_STRING("value props"),
                                &dom->propValueRegistry);

    printf("boolean property nodes inside DOM...\n");
    printf("total number of boolean properties: %zu\n", dom->boolProps.len);
    for (ptrdiff_t i = 0; i < dom->boolProps.len; i++) {
        flo_html_BooleanProperty boolProps = dom->boolProps.buf[i];
        printf("node ID: %-5u prop ID: %-5u\n", boolProps.nodeID,
               boolProps.propID);
    }
    printf("\n");

    printf("key-value property nodes inside DOM...\n");
    printf("total number of key-value properties: %zu\n", dom->props.len);
    for (ptrdiff_t i = 0; i < dom->props.len; i++) {
        flo_html_Property property = dom->props.buf[i];
        printf("node ID: %-5u key ID: %-5u value ID: %-5u\n", property.nodeID,
               property.keyID, property.valueID);
    }
    printf("\n");

    printf("parent-first-child inside DOM...\n");
    printf("total number of parent-first-child: %zu\n",
           dom->parentFirstChilds.len);
    for (ptrdiff_t i = 0; i < dom->parentFirstChilds.len; i++) {
        printf("parent node ID: %-5u first child node ID: %-5u\n",
               dom->parentFirstChilds.buf[i].parentID,
               dom->parentFirstChilds.buf[i].childID);
    }
    printf("\n");

    printf("parent-child inside DOM...\n");
    printf("total number of parent-child: %zu\n", dom->parentChilds.len);
    for (ptrdiff_t i = 0; i < dom->parentChilds.len; i++) {
        printf("parent: %-5u child: %-5u\n", dom->parentChilds.buf[i].parentID,
               dom->parentChilds.buf[i].childID);
    }
    printf("\n");

    printf("next nodes inside DOM...\n");
    printf("total number of next nodes: %zu\n", dom->nextNodes.len);
    for (ptrdiff_t i = 0; i < dom->nextNodes.len; i++) {
        printf("current node: %-5u next node: %-5u\n",
               dom->nextNodes.buf[i].currentNodeID,
               dom->nextNodes.buf[i].nextNodeID);
    }
    printf("\n\n");
}
