#ifndef DOM_DOCUMENT_H
#define DOM_DOCUMENT_H

#include "document-status.h"
#include "type/node/boolean-property.h"
#include "type/node/next-node.h"
#include "type/node/node.h"
#include "type/node/parent-first-child.h"
#include "type/node/property.h"
#include "type/node/text-node.h"
#include "utils/file/file-status.h"

#define NODES_PAGE_SIZE (1U << 10U)
#define NODES_PER_PAGE (NODES_PAGE_SIZE / sizeof(Node))

#define PARENT_CHILDS_PAGE_SIZE (1U << 8U)
#define PARENT_CHILDS_PER_PAGE                                                 \
    (PARENT_CHILDS_PAGE_SIZE / sizeof(ParentFirstChild))

#define NEXT_NODES_PAGE_SIZE (1U << 8U)
#define NEXT_NODES_PER_PAGE (NEXT_NODES_PAGE_SIZE / sizeof(NextNode))

#define BOOLEAN_PROPERTIES_PAGE_SIZE (1U << 8U)
#define BOOLEAN_PROPERTIES_PER_PAGE                                            \
    (BOOLEAN_PROPERTIES_PAGE_SIZE / sizeof(BooleanProperty))

#define PROPERTIES_PAGE_SIZE (1U << 8U)
#define PROPERTIES_PER_PAGE (PROPERTIES_PAGE_SIZE / sizeof(Property))

#define TEXT_NODES_PAGE_SIZE (1U << 8U)
#define TEXT_NODES_PER_PAGE (TEXT_NODES_PAGE_SIZE / sizeof(TextNode))

typedef struct {
    Node *first;

    Node *nodes;
    size_t nodeLen;
    size_t nodeCap;

    ParentFirstChild *parentFirstChilds;
    size_t parentFirstChildLen;
    size_t parentFirstChildCap;

    NextNode *nextNodes;
    size_t nextNodeLen;
    size_t nextNodeCap;

    BooleanProperty *boolProps;
    size_t boolPropsLen;
    size_t boolPropsCap;

    Property *props;
    size_t propsLen;
    size_t propsCap;

    TextNode *text;
    size_t textLen;
    size_t textCap;
} __attribute__((aligned(128))) Document;

DocumentStatus createDocument(const char *xmlString, Document *doc);

DocumentStatus addNode(node_id *nodeID, element_id tagID, Document *doc);
DocumentStatus addParentFirstChild(node_id parentID, node_id childID,
                                   Document *doc);
DocumentStatus addNextNode(node_id currentNodeID, node_id nextNodeID,
                           Document *doc);
DocumentStatus addBooleanProperty(node_id nodeID, element_id propID,
                                  Document *doc);
DocumentStatus addProperty(node_id nodeID, element_id keyID, element_id valueID,
                           Document *doc);
DocumentStatus addTextNode(node_id nodeID, element_id textID, Document *doc);
node_id getFirstChild(node_id parentID, const Document *doc);
node_id getNextNode(node_id currentNodeID, const Document *doc);

void destroyDocument(const Document *doc);

#endif
