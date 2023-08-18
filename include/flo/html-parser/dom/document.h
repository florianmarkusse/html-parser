#ifndef FLO_HTML_PARSER_DOM_DOCUMENT_H
#define FLO_HTML_PARSER_DOM_DOCUMENT_H

#include "document-status.h"
#include "flo/html-parser/type/node/boolean-property.h"
#include "flo/html-parser/type/node/next-node.h"
#include "flo/html-parser/type/node/node.h"
#include "flo/html-parser/type/node/parent-child.h"
#include "flo/html-parser/type/node/parent-first-child.h"
#include "flo/html-parser/type/node/property.h"
#include "flo/html-parser/type/node/text-node.h"
#include "flo/html-parser/utils/file/file-status.h"

#define NODES_PAGE_SIZE (1U << 10U)
#define NODES_PER_PAGE (NODES_PAGE_SIZE / sizeof(Node))

#define ERROR_NODE_ID 0

#define PARENT_FIRST_CHILDS_PAGE_SIZE (1U << 8U)
#define PARENT_FIRST_CHILDS_PER_PAGE                                           \
    (PARENT_FIRST_CHILDS_PAGE_SIZE / sizeof(ParentFirstChild))

#define PARENT_CHILDS_PAGE_SIZE (1U << 8U)
#define PARENT_CHILDS_PER_PAGE (PARENT_CHILDS_PAGE_SIZE / sizeof(ParentChild))

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
    node_id firstNodeID;

    Node *nodes;
    size_t nodeLen;
    size_t nodeCap;

    ParentFirstChild *parentFirstChilds;
    size_t parentFirstChildLen;
    size_t parentFirstChildCap;

    ParentChild *parentChilds;
    size_t parentChildLen;
    size_t parentChildCap;

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

DocumentStatus createDocument(const char *htmlString, Document *doc,
                              DataContainer *dataContainer);

DocumentStatus createNode(node_id *nodeID, Document *doc);
DocumentStatus setTagID(node_id nodeID, element_id tagID, Document *doc);
DocumentStatus addNode(node_id *nodeID, element_id tagID, Document *doc);
DocumentStatus addParentFirstChild(node_id parentID, node_id childID,
                                   Document *doc);
DocumentStatus addParentChild(node_id parentID, node_id childID, Document *doc);
DocumentStatus addNextNode(node_id currentNodeID, node_id nextNodeID,
                           Document *doc);
DocumentStatus addBooleanProperty(node_id nodeID, element_id propID,
                                  Document *doc);
DocumentStatus addProperty(node_id nodeID, element_id keyID, element_id valueID,
                           Document *doc);
DocumentStatus addTextNode(node_id nodeID, element_id textID, Document *doc);
DocumentStatus replaceTextNode(node_id nodeID, element_id newTextID,
                               Document *doc);
node_id getFirstChild(node_id parentID, const Document *doc);
node_id getNextNode(node_id currentNodeID, const Document *doc);

void destroyDocument(const Document *doc);

#endif