#include <stdio.h>
#include <string.h>

#include "file/path.h"
#include "flo/html-parser/dom/dom-util.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/node/text-node.h"
#include "log.h"

void printNode(flo_html_node_id nodeID, ptrdiff_t indentation,
               flo_html_Dom *dom, flo_WriteBuffer *buffer) {
    flo_html_Node node = dom->nodes.buf[nodeID];

    if (node.nodeType == NODE_TYPE_ERROR) {
        return;
    }

    if (node.nodeType == NODE_TYPE_ROOT) {
        flo_html_node_id childNode = flo_html_getFirstChild(nodeID, dom);
        while (childNode) {
            printNode(childNode, indentation + 1, dom, buffer);
            childNode = flo_html_getNext(childNode, dom);
        }
        return;
    }

    if (node.nodeType == NODE_TYPE_TEXT) {
        FLO_LOG_DATA(node.text, buffer);
        return;
    }

    flo_String tag = dom->tagRegistry.buf[node.tagID].tag;
    FLO_LOG_DATA("<", buffer);
    FLO_LOG_DATA(tag, buffer);

    for (ptrdiff_t i = 0; i < dom->boolProps.len; i++) {
        flo_html_BooleanProperty boolProp = dom->boolProps.buf[i];

        if (boolProp.nodeID == node.nodeID) {
            FLO_LOG_DATA(" ", buffer);
            FLO_LOG_DATA(dom->boolPropRegistry.buf[boolProp.propID], buffer, 0);
        }
    }

    for (ptrdiff_t i = 0; i < dom->props.len; i++) {
        flo_html_Property prop = dom->props.buf[i];

        if (prop.nodeID == node.nodeID) {
            FLO_LOG_DATA(" ", buffer);
            FLO_LOG_DATA(dom->propKeyRegistry.buf[prop.keyID], buffer, 0);
            FLO_LOG_DATA("=\"", buffer);
            FLO_LOG_DATA(dom->propValueRegistry.buf[prop.valueID], buffer, 0);
            FLO_LOG_DATA("\"", buffer);
        }
    }

    flo_html_TagRegistration *tagRegistration =
        &dom->tagRegistry.buf[node.tagID];
    if (!tagRegistration->isPaired) {
        if (flo_stringEquals(tag, FLO_STRING("!DOCTYPE"))) {
            FLO_LOG_DATA(">", buffer);
        } else {
            FLO_LOG_DATA("/>", buffer);
        }
        return;
    }
    FLO_LOG_DATA(">", buffer);

    flo_html_node_id childNode = flo_html_getFirstChild(nodeID, dom);
    while (childNode) {
        printNode(childNode, indentation + 1, dom, buffer);
        childNode = flo_html_getNext(childNode, dom);
    }

    FLO_LOG_DATA("</", buffer);
    FLO_LOG_DATA(tag, buffer);
    FLO_LOG_DATA(">", buffer);
}

void flo_html_printHTML(flo_html_Dom *dom) {
    FLO_FLUSH_AFTER(FLO_STDOUT) {
        FLO_INFO((FLO_STRING("printing HTML...\n\n")));
        flo_html_node_id currentNodeID = FLO_HTML_ROOT_NODE_ID;
        while (currentNodeID) {
            printNode(currentNodeID, 0, dom, flo_getWriteBuffer(FLO_STDOUT));
            currentNodeID = flo_html_getNext(currentNodeID, dom);
        }
        FLO_INFO((FLO_STRING("\n\n")));
    }
}

flo_FileStatus flo_html_writeHTMLToFile(flo_html_Dom *dom, flo_String filePath,
                                        flo_Arena scratch) {
    flo_createPath(filePath, scratch);
    FILE *file = fopen((char *)filePath.buf, "wbe");
    if (file == NULL) {
        FLO_FLUSH_AFTER(FLO_STDERR) {
            FLO_ERROR("Failed to open file for writing: ");
            FLO_ERROR(filePath, FLO_NEWLINE);
        }
        return FILE_CANT_OPEN;
    }

    ptrdiff_t fileBufferLen = 1 << 13;
    unsigned char *fileBuffer = FLO_NEW(&scratch, unsigned char, fileBufferLen);
    flo_WriteBuffer writeBuffer = {.fileDescriptor = fileno(file),
                                   .cap = fileBufferLen,
                                   .buf = fileBuffer,
                                   .len = 0};

    flo_html_node_id currentNodeID = FLO_HTML_ROOT_NODE_ID;
    while (currentNodeID) {
        printNode(currentNodeID, 0, dom, &writeBuffer);
        currentNodeID = flo_html_getNext(currentNodeID, dom);
    }

    flo_flushBuffer(&writeBuffer);

    fclose(file);

    return FILE_SUCCESS;
}

// Does not automatically flush!!!!!!!
void printflo_html_BasicRegistry(flo_String registryName,
                                 flo_String_d_a *strings) {
    FLO_INFO((FLO_STRING("registration nodes inside DOM for registry: ")));
    FLO_INFO(registryName, FLO_NEWLINE);

    FLO_INFO("Total number of nodes: ");
    FLO_INFO(strings->len, FLO_NEWLINE);
    for (ptrdiff_t i = 0; i < strings->len; i++) {
        flo_String string = strings->buf[i];
        FLO_INFO("ID: ");
        flo_appendPtrDiffToBufferMinSize(i, 3, flo_getWriteBuffer(FLO_STDOUT),
                                         0);
        FLO_INFO(" value: ");
        FLO_INFO(string, FLO_NEWLINE);
    }

    FLO_INFO("\n");
}

// Does not automatically flush!!!!!!!!
void printStringAutoUint16Map(flo_Arena scratch,
                              flo_trie_StringAutoUint16Node *node) {
    flo_trie_StringAutoUint16Data data;
    FLO_FOR_EACH_TRIE_STRING_AUTO_UINT16(data, node, scratch) {
        FLO_INFO((FLO_STRING("string = ")));
        flo_appendToBufferMinSize(data.key, 30, flo_getWriteBuffer(FLO_STDOUT),
                                  0);
        FLO_INFO((FLO_STRING(" with id = ")));
        FLO_INFO(data.value, FLO_NEWLINE);
    }
    FLO_INFO((FLO_STRING("\n")));
}

void flo_html_printDomStatus(flo_html_Dom *dom, flo_Arena scratch) {
    FLO_FLUSH_AFTER(FLO_STDOUT) {
        FLO_INFO((FLO_STRING("printing DOM status...\n\n")));

        FLO_INFO((FLO_STRING("printing property status...\n")));

        FLO_INFO((FLO_STRING("printing keys...\n")));
        printStringAutoUint16Map(scratch, dom->propKeyMap.node);

        FLO_INFO((FLO_STRING("printing values...\n")));
        printStringAutoUint16Map(scratch, dom->propValueMap.node);

        FLO_INFO((FLO_STRING("printing bool property status...\n")));
        printStringAutoUint16Map(scratch, dom->boolPropMap.node);

        FLO_INFO((FLO_STRING("printing tags status...\n")));
        printStringAutoUint16Map(scratch, dom->tagMap.node);

        FLO_INFO((FLO_STRING("Printing DOM contents...\n")));

        FLO_INFO((FLO_STRING("Nodes inside DOM...\n")));
        FLO_INFO((FLO_STRING("Total number of nodes: ")));
        FLO_INFO(dom->nodes.len, FLO_NEWLINE);
        for (ptrdiff_t i = 0; i < dom->nodes.len; i++) {
            flo_html_Node node = dom->nodes.buf[i];

            if (node.nodeType == NODE_TYPE_DOCUMENT) {
                FLO_INFO((FLO_STRING("Node ID: ")));
                flo_appendUint64ToBufferMinSize(
                    node.nodeID, 5, flo_getWriteBuffer(FLO_STDOUT), 0);
                FLO_INFO((FLO_STRING(" Node type: ")));
                flo_appendToBufferMinSize(
                    flo_html_nodeTypeToString(node.nodeType), 10,
                    flo_getWriteBuffer(FLO_STDOUT), 0);
                FLO_INFO((FLO_STRING(" with tag ID: ")));
                FLO_INFO(node.tagID, FLO_NEWLINE);
            } else {
                FLO_INFO((FLO_STRING("Node ID: ")));
                flo_appendUint64ToBufferMinSize(
                    node.nodeID, 5, flo_getWriteBuffer(FLO_STDOUT), 0);
                FLO_INFO((FLO_STRING(" Node type: ")));
                flo_appendToBufferMinSize(
                    flo_html_nodeTypeToString(node.nodeType), 10,
                    flo_getWriteBuffer(FLO_STDOUT), FLO_NEWLINE);
            }
        }
        FLO_INFO("\n");

        FLO_INFO("tag registration nodes inside DOM...\n");

        FLO_INFO("total number of tag registration nodes: ");
        FLO_INFO(dom->tagRegistry.len, FLO_NEWLINE);
        for (ptrdiff_t i = 0; i < dom->tagRegistry.len; i++) {
            flo_html_TagRegistration tagRegistration = dom->tagRegistry.buf[i];

            FLO_INFO("tag ID: ");
            flo_appendUint64ToBufferMinSize(i, 5,
                                            flo_getWriteBuffer(FLO_STDOUT), 0);
            FLO_INFO((FLO_STRING(" tag: ")));
            flo_appendToBufferMinSize(tagRegistration.tag, 20,
                                      flo_getWriteBuffer(FLO_STDOUT), 0);
            FLO_INFO(" isPaired: ");
            FLO_INFO(tagRegistration.isPaired, FLO_NEWLINE);
        }
        FLO_INFO("\n");

        printflo_html_BasicRegistry(FLO_STRING("bool props"),
                                    &dom->boolPropRegistry);
        printflo_html_BasicRegistry(FLO_STRING("key props"),
                                    &dom->propKeyRegistry);
        printflo_html_BasicRegistry(FLO_STRING("value props"),
                                    &dom->propValueRegistry);

        FLO_INFO("boolean property nodes inside DOM...\n");
        FLO_INFO("total number of boolean properties: ");
        FLO_INFO(dom->boolProps.len, FLO_NEWLINE);
        for (ptrdiff_t i = 0; i < dom->boolProps.len; i++) {
            flo_html_BooleanProperty boolProps = dom->boolProps.buf[i];

            FLO_INFO("node ID: ");
            flo_appendPtrDiffToBufferMinSize(boolProps.nodeID, 3,
                                             flo_getWriteBuffer(FLO_STDOUT), 0);
            FLO_INFO(" prop ID: ");
            FLO_INFO(boolProps.propID, FLO_NEWLINE);
        }
        FLO_INFO("\n");

        FLO_INFO("key-value property nodes inside DOM...\n");
        FLO_INFO("total number of key-value properties: ");
        FLO_INFO(dom->props.len, FLO_NEWLINE);
        for (ptrdiff_t i = 0; i < dom->props.len; i++) {
            flo_html_Property property = dom->props.buf[i];
            FLO_INFO("node ID: ");
            flo_appendPtrDiffToBufferMinSize(property.nodeID, 3,
                                             flo_getWriteBuffer(FLO_STDOUT), 0);
            FLO_INFO(" key ID: ");
            flo_appendPtrDiffToBufferMinSize(property.keyID, 3,
                                             flo_getWriteBuffer(FLO_STDOUT), 0);
            FLO_INFO(" value ID: ");
            FLO_INFO(property.valueID, FLO_NEWLINE);
        }
        FLO_INFO("\n");

        FLO_INFO("parent-first-child inside DOM...\n");
        FLO_INFO("total number of parent-first-child: ");
        FLO_INFO(dom->parentFirstChilds.len, FLO_NEWLINE);
        for (ptrdiff_t i = 0; i < dom->parentFirstChilds.len; i++) {
            FLO_INFO("parent node ID: ");
            flo_appendUint64ToBufferMinSize(
                dom->parentFirstChilds.buf[i].parentID, 5,
                flo_getWriteBuffer(FLO_STDOUT), 0);
            FLO_INFO(" first child node ID: ");
            flo_appendUint64ToBufferMinSize(
                dom->parentFirstChilds.buf[i].childID, 5,
                flo_getWriteBuffer(FLO_STDOUT), FLO_NEWLINE);
        }
        FLO_INFO("\n");

        FLO_INFO("parent-child inside DOM...\n");
        FLO_INFO("total number of parent-child: ");
        FLO_INFO(dom->parentChilds.len, FLO_NEWLINE);
        for (ptrdiff_t i = 0; i < dom->parentChilds.len; i++) {
            FLO_INFO("parent: ");
            flo_appendUint64ToBufferMinSize(dom->parentChilds.buf[i].parentID,
                                            5, flo_getWriteBuffer(FLO_STDOUT),
                                            0);
            FLO_INFO(" child: ");
            flo_appendUint64ToBufferMinSize(dom->parentChilds.buf[i].childID, 5,
                                            flo_getWriteBuffer(FLO_STDOUT),
                                            FLO_NEWLINE);
        }
        FLO_INFO("\n");

        FLO_INFO("next nodes inside DOM...\n");
        FLO_INFO("total number of next nodes: ");
        FLO_INFO(dom->nextNodes.len, FLO_NEWLINE);
        for (ptrdiff_t i = 0; i < dom->nextNodes.len; i++) {
            FLO_INFO("current node: ");
            flo_appendUint64ToBufferMinSize(dom->nextNodes.buf[i].currentNodeID,
                                            5, flo_getWriteBuffer(FLO_STDOUT),
                                            0);
            FLO_INFO(" next node: ");
            flo_appendUint64ToBufferMinSize(dom->nextNodes.buf[i].nextNodeID, 5,
                                            flo_getWriteBuffer(FLO_STDOUT),
                                            FLO_NEWLINE);
        }
        FLO_INFO("\n\n");
    }
}
