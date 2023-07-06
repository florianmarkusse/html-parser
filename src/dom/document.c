#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "dom/document.h"
#include "tokenizer/parse.h"

DocumentStatus createDocument(const char *xmlString, Document *doc) {
    doc->nodes = malloc(NODES_PAGE_SIZE);
    doc->nodeLen = 0;

    doc->parentFirstChilds = malloc(PARENT_CHILDS_PAGE_SIZE);
    doc->parentFirstChildLen = 0;

    doc->nextNodes = malloc(NEXT_NODE_PAGE_SIZE);
    doc->nextNodeLen = 0;

    if (doc->nodes == NULL || doc->parentFirstChilds == NULL ||
        doc->nextNodes == NULL) {
        fprintf(stderr, "Failed to allocate memory for nodes.\n");
        destroyDocument(doc);
        return DOCUMENT_ERROR_MEMORY;
    }

    parse(xmlString, doc);

    return DOCUMENT_SUCCESS;
}

node_id addNode(const tag_id tagID, Document *doc) {
    Node *newNode = &(doc->nodes[doc->nodeLen]);
    newNode->nodeID =
        doc->nodeLen + 1; // We start at 1 because we need to
                          // initialize variables when parsing at 0.
    newNode->tagID = tagID;
    doc->nodeLen++;
    return newNode->nodeID;
}

void addParentFirstChild(node_id parentID, node_id childID, Document *doc) {
    ParentFirstChild *newParentFirstChild =
        &(doc->parentFirstChilds[doc->parentFirstChildLen]);
    newParentFirstChild->parentID = parentID;
    newParentFirstChild->childID = childID;
    doc->parentFirstChildLen++;
}

void addNextNode(node_id currentNodeID, node_id nextNodeID, Document *doc) {
    NextNode *newNextNode = &(doc->nextNodes[doc->nextNodeLen]);
    newNextNode->currentNodeID = currentNodeID;
    newNextNode->nextNodeID = nextNodeID;
    doc->nextNodeLen++;
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
    printf("\n");
}

void printNode(const Document *doc, const node_id nodeID) {
    Node node = doc->nodes[nodeID];
    const char *tag = globalTags.tags[nodeID];
    if (isSelfClosing(node.tagID)) {
        printf("<%s my ID: %hu />\n", tag, node.nodeID);
        return;
    }
}

void printDocument(const Document *doc) {
    printf("Printing document...\n\n");
    for (size_t i = 0; i < doc->nodeLen; i++) {
        Node node = doc->nodes[i];
        const char *type = globalTags.tags[node.tagID];

        printBits(node.tagID);

        if (isSelfClosing(node.tagID)) {
            printf("<%s my ID: %hu />\n", type, node.nodeID);
        } else {
            printf("<%s> my ID: %hu </%s>\n", type, node.nodeID, type);
        }

        printf("\n");
    }

    for (size_t i = 0; i < doc->parentFirstChildLen; i++) {
        printf("Parent: %hu with first child: %hu\n",
               doc->parentFirstChilds[i].parentID,
               doc->parentFirstChilds[i].childID);
    }

    for (size_t i = 0; i < doc->nextNodeLen; i++) {
        printf("current node: %hu with next node: %hu\n",
               doc->nextNodes[i].currentNodeID, doc->nextNodes[i].nextNodeID);
    }
}
