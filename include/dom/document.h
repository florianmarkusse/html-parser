#ifndef DOM_DOCUMENT_H
#define DOM_DOCUMENT_H

#include "document-status.h"
#include "type/next-node.h"
#include "type/node-attribute-value.h"
#include "type/node-attribute.h"
#include "type/node.h"
#include "type/parent-first-child.h"
#include "utils/file/file-status.h"

#define NODES_PAGE_SIZE (1U << 10U)
#define NODES_PER_PAGE (NODES_PAGE_SIZE / sizeof(Node))

#define PARENT_CHILDS_PAGE_SIZE (1U << 8U)
#define PARENT_CHILDS_PER_PAGE                                                 \
    (PARENT_CHILDS_PAGE_SIZE / sizeof(ParentFirstChild))

#define NEXT_NODES_PAGE_SIZE (1U << 8U)
#define NEXT_NODES_PER_PAGE (NEXT_NODES_PAGE_SIZE / sizeof(NextNode))

#define ATTRIBUTE_NODES_PAGE_SIZE (1U << 8U)
#define ATTRIBUTE_NODES_PER_PAGE                                               \
    (ATTRIBUTE_NODES_PAGE_SIZE / sizeof(NodeAttribute))

#define ATTRIBUTE_VALUE_NODES_PAGE_SIZE (1U << 8U)
#define ATTRIBUTE_VALUE_NODES_PER_PAGE                                         \
    (ATTRIBUTE_VALUE_NODES_PAGE_SIZE / sizeof(NodeAttribute))

typedef struct {
    Node *nodes;
    size_t nodeLen;
    size_t nodeCapacity;

    ParentFirstChild *parentFirstChilds;
    size_t parentFirstChildLen;
    size_t parentFirstChildCapacity;

    NextNode *nextNodes;
    size_t nextNodeLen;
    size_t nextNodeCapacity;

    NodeAttribute *nodeAttributes;
    size_t nodeAttributeLen;
    size_t nodeAttributeCapacity;

    NodeAttributeValue *nodeAttributeValues;
    size_t nodeAttributeValueLen;
    size_t nodeAttributeValueCapacity;
} __attribute__((aligned(128))) Document;

DocumentStatus createDocument(const char *xmlString, Document *doc);

DocumentStatus addNode(node_id *nodeID, element_id tagID, Document *doc);
DocumentStatus addParentFirstChild(node_id parentID, node_id childID,
                                   Document *doc);
DocumentStatus addNextNode(node_id currentNodeID, node_id nextNodeID,
                           Document *doc);
DocumentStatus addAttributeNode(node_id nodeID, element_id attributeID,
                                Document *doc);

node_id getFirstChild(node_id parentID, const Document *doc);
node_id getNextNode(node_id currentNodeID, const Document *doc);

void destroyDocument(const Document *doc);

#endif
