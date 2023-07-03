#ifndef DOM_DOCUMENT_H
#define DOM_DOCUMENT_H

#include "type/next-node.h"
#include "type/node.h"
#include "type/parent-first-child.h"

typedef struct {
    Node *nodes;
    unsigned int nodeLen;

    ParentFirstChild *parentFirstChilds;
    unsigned int parentFirstChildLen;

    NextNode *nextNodes;
    unsigned int nextNodeLen;

} __attribute__((aligned(64))) Document;

Document createDocument(const char *hmtlString);

unsigned int addNode(NodeType nodeType, Document *doc);
void addParentFirstChild(unsigned int parentID, unsigned int childID,
                         Document *doc);
void addNextNode(unsigned int currentNodeID, unsigned int nextNodeID,
                 Document *doc);

void destroyDocument(const Document *doc);

void printDocument(const Document *doc);

#endif
