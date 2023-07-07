#ifndef DOM_DOCUMENT_H
#define DOM_DOCUMENT_H

#include "document-status.h"
#include "type/next-node.h"
#include "type/node.h"
#include "type/parent-first-child.h"

#define NODES_PAGE_SIZE (1U << 10U)
#define NODES_PER_PAGE (NODES_PAGE_SIZE / sizeof(Node))

#define PARENT_CHILDS_PAGE_SIZE (1U << 8U)
#define PARENT_CHILDS_PER_PAGE (NODES_PAGE_SIZE / sizeof(ParentChilds))

#define NEXT_NODE_PAGE_SIZE (1U << 8U)
#define NEXT_NODES_PER_PAGE (NODES_PAGE_SIZE / sizeof(NextNode))

typedef struct {
    Node *nodes;
    size_t nodeLen;

    ParentFirstChild *parentFirstChilds;
    size_t parentFirstChildLen;

    NextNode *nextNodes;
    size_t nextNodeLen;

} __attribute__((aligned(64))) Document;

DocumentStatus createDocument(const char *xmlString, Document *doc);

DocumentStatus addNode(node_id *nodeID, tag_id tagID, Document *doc);
DocumentStatus addParentFirstChild(node_id parentID, node_id childID,
                                   Document *doc);
DocumentStatus addNextNode(node_id currentNodeID, node_id nextNodeID,
                           Document *doc);

void destroyDocument(const Document *doc);

void printDocument(const Document *doc);

#endif
