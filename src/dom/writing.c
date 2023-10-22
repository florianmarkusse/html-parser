#include <stdio.h>
#include <string.h>

#include "flo/html-parser/dom/dom-util.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/node/text-node.h"
#include "flo/html-parser/util/file/path.h"

void printNode(const flo_html_node_id nodeID, const ptrdiff_t indentation,
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
        fprintf(output, "%.*s", FLO_HTML_S_P(node.text));
        return;
    }

    const flo_html_String tag = flo_html_getStringFromHashSet(
        &dom->tags, dom->tagRegistry.buf[node.tagID].hashElement);
    fprintf(output, "<%.*s", FLO_HTML_S_P(tag));

    for (ptrdiff_t i = 0; i < dom->boolProps.len; i++) {
        flo_html_BooleanProperty boolProp = dom->boolProps.buf[i];

        if (boolProp.nodeID == node.nodeID) {
            const flo_html_String prop = flo_html_getStringFromHashSet(
                &dom->boolPropsSet, dom->boolPropRegistry.buf[boolProp.propID]);
            fprintf(output, " %.*s", FLO_HTML_S_P(prop));
        }
    }

    for (ptrdiff_t i = 0; i < dom->props.len; i++) {
        flo_html_Property prop = dom->props.buf[i];

        if (prop.nodeID == node.nodeID) {
            const flo_html_String key = flo_html_getStringFromHashSet(
                &dom->propKeys, dom->propKeyRegistry.buf[prop.keyID]);
            const flo_html_String value = flo_html_getStringFromHashSet(
                &dom->propValues, dom->propValueRegistry.buf[prop.valueID]);
            fprintf(output, " %.*s=\"%.*s\"", FLO_HTML_S_P(key),
                    FLO_HTML_S_P(value));
        }
    }

    flo_html_TagRegistration *tagRegistration =
        &dom->tagRegistry.buf[node.tagID];
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
        printNode(childNode, indentation + 1, dom, output);
        childNode = flo_html_getNext(childNode, dom);
    }

    fprintf(output, "</%.*s>", FLO_HTML_S_P(tag));
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

flo_html_FileStatus flo_html_writeHTMLToFile(flo_html_Dom *dom,
                                             const flo_html_String filePath,
                                             flo_html_Arena scratch) {
    flo_html_createPath(filePath, scratch);
    // casting here because filePath should not contain any funny characters.
    FILE *file = fopen((char *)filePath.buf, "wbe");
    if (file == NULL) {
        printf("Failed to open file for writing: %.*s\n",
               FLO_HTML_S_P(filePath));
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

void printflo_html_BasicRegistry(const flo_html_String registryName,
                                 const flo_html_HashElement_d_a *hashElements,
                                 const flo_html_StringHashSet *set) {
    printf("%-20.*s\nregistration nodes inside DOM...\n",
           FLO_HTML_S_P(registryName));
    printf("total number of nodes: %zu\n", hashElements->len);
    for (ptrdiff_t i = 0; i < hashElements->len; i++) {
        flo_html_HashElement hashElement = hashElements->buf[i];
        const flo_html_String value =
            flo_html_getStringFromHashSet(set, hashElement);
        printf("ID: %zu value: %-20.*s hash: %zu offset: %u\n", i,
               FLO_HTML_S_P(value), hashElement.hash, hashElement.offset);
    }
    printf("\n");
}

void printSetStatus(const flo_html_StringHashSet *set) {
    printf("hash set contents...\n");
    flo_html_StringHashSetIterator iterator =
        (flo_html_StringHashSetIterator){.set = set, .index = 0};

    flo_html_String string;
    while ((string = flo_html_nextStringHashSetIterator(&iterator)).len != 0) {
        printf("%.*s\n", FLO_HTML_S_P(string));
    }
}

void flo_html_printDomStatus(flo_html_Dom *dom) {
    printf("printing DOM status...\n\n");

    printf("printing property status...\n\n");
    printf("printing keys...\n");
    printSetStatus(&dom->propKeys);
    printf("printing values...\n");
    printSetStatus(&dom->propValues);

    printf("printing bool property status...\n\n");
    printSetStatus(&dom->boolPropsSet);

    printf("printing tags status...\n\n");
    printSetStatus(&dom->tags);

    printf("Printing DOM contents...\n");

    printf("nodes inside DOM...\n");
    printf("total number of nodes: %zu\n", dom->nodes.len);
    for (ptrdiff_t i = 0; i < dom->nodes.len; i++) {
        flo_html_Node node = dom->nodes.buf[i];

        if (node.nodeType == NODE_TYPE_DOCUMENT) {
            printf("node ID: %-5u node type: %-10.*s with tag ID: %-5u\n",
                   node.nodeID,
                   FLO_HTML_S_P(flo_html_nodeTypeToString(node.nodeType)),
                   node.tagID);
        } else {
            printf("node ID: %-5u node type: %-10.*s\n", node.nodeID,
                   FLO_HTML_S_P(flo_html_nodeTypeToString(node.nodeType)));
        }
    }
    printf("\n");

    printf("tag registration nodes inside DOM...\n");
    printf("total number of tag registration nodes: %zu\n",
           dom->tagRegistry.len);
    for (ptrdiff_t i = 0; i < dom->tagRegistry.len; i++) {
        flo_html_TagRegistration tagRegistration = dom->tagRegistry.buf[i];
        const flo_html_String tag = flo_html_getStringFromHashSet(
            &dom->tags, tagRegistration.hashElement);
        printf("tag ID: %-5td tag: %-20.*s isPaired: %d hash: %zu offset: %u\n",
               i, FLO_HTML_S_P(tag), tagRegistration.isPaired,
               tagRegistration.hashElement.hash,
               tagRegistration.hashElement.offset);
    }
    printf("\n");

    printflo_html_BasicRegistry(FLO_HTML_S("bool props"),
                                &dom->boolPropRegistry, &dom->boolPropsSet);
    printflo_html_BasicRegistry(FLO_HTML_S("key props"), &dom->propKeyRegistry,
                                &dom->propKeys);
    printflo_html_BasicRegistry(FLO_HTML_S("value props"),
                                &dom->propValueRegistry, &dom->propValues);

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
