#include <stdio.h>
#include <stdlib.h>

#include "dom/document.h"
#include "tokenizer/parse.h"

#define NODES_PAGE_SIZE (1U << 10U)
#define NODES_PER_PAGE (NODES_PAGE_SIZE / sizeof(Node))

#define PARENT_CHILDS_PAGE_SIZE (1U << 8U)
#define PARENT_CHILDS_PER_PAGE (NODES_PAGE_SIZE / sizeof(ParentChilds))

#define NEXT_NODE_PAGE_SIZE (1U << 8U)
#define NEXT_NODES_PER_PAGE (NODES_PAGE_SIZE / sizeof(NextNode))

Document createDocument(const char *hmtlString) {
    Document document;
    document.nodes = malloc(NODES_PAGE_SIZE);
    document.nodeLen = 0;

    document.parentFirstChilds = malloc(PARENT_CHILDS_PAGE_SIZE);
    document.parentFirstChildLen = 0;

    document.nextNodes = malloc(NEXT_NODE_PAGE_SIZE);
    document.nextNodeLen = 0;

    parseNodesNew(hmtlString, &document);

    return document;
}

unsigned int addNode(const NodeType type, Document *doc) {
    Node *newNode = &(doc->nodes[doc->nodeLen]);
    newNode->ID = doc->nodeLen;
    newNode->type = type;
    doc->nodeLen++;
    return newNode->ID;
}

void addParentFirstChild(unsigned int parentID, unsigned int childID,
                         Document *doc) {
    ParentFirstChild *newParentFirstChild =
        &(doc->parentFirstChilds[doc->parentFirstChildLen]);
    newParentFirstChild->parentID = parentID;
    newParentFirstChild->childID = childID;
    doc->parentFirstChildLen++;
}

void addNextNode(unsigned int currentNodeID, unsigned int nextNodeID,
                 Document *doc) {
    NextNode *newNextNode = &(doc->nextNodes[doc->nextNodeLen]);
    newNextNode->currentNodeID = currentNodeID;
    newNextNode->nextNodeID = nextNodeID;
    doc->nextNodeLen++;
}

void destroyDocument(const Document *doc) {
    free((void *)doc->nodes);
    free((void *)doc->parentFirstChilds);
}

void printBits(const NodeType type) {
    size_t numBits = sizeof(NodeType) * 8;
    for (size_t i = 0; i < numBits; i++) {
        unsigned int bit = (type >> (numBits - 1 - i)) & 1;
        printf("%u", bit);
    }
    printf("\n");
}

void printNode(const Document *doc, const unsigned int nodeID) {
    Node node = doc->nodes[nodeID];
    const char *type = mapTypeToString(node.type);
    if (isSelfClosing(node.type)) {
        printf("<%s my ID: %u />\n", type, node.ID);
        return;
    }
}

void printDocument(const Document *doc) {
    printf("Printing document...\n\n");
    for (unsigned int i = 0; i < doc->nodeLen; i++) {
        Node node = doc->nodes[i];
        const char *type = mapTypeToString(node.type);

        printBits(node.type);

        if (isSelfClosing(node.type)) {
            printf("<%s my ID: %u />\n", type, node.ID);
        } else {
            printf("<%s> my ID: %u </%s>\n", type, node.ID, type);
        }

        printf("\n");
    }

    for (unsigned int i = 0; i < doc->parentFirstChildLen; i++) {
        printf("Parent: %u with first child: %u\n",
               doc->parentFirstChilds[i].parentID,
               doc->parentFirstChilds[i].childID);
    }

    for (unsigned int i = 0; i < doc->nextNodeLen; i++) {
        printf("current node: %u with next node: %u\n",
               doc->nextNodes[i].currentNodeID, doc->nextNodes[i].nextNodeID);
    }
}
