#ifndef FLO_HTML_PARSER_DOM_DOM_H
#define FLO_HTML_PARSER_DOM_DOM_H

#include "dom-status.h"
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
} __attribute__((aligned(128))) Dom;

DomStatus createDom(const char *htmlString, Dom *dom,
                    DataContainer *dataContainer);

DomStatus createNode(node_id *nodeID, Dom *dom);
DomStatus setTagID(node_id nodeID, element_id tagID, Dom *dom);
DomStatus addNode(node_id *nodeID, element_id tagID, Dom *dom);
DomStatus addParentFirstChild(node_id parentID, node_id childID, Dom *dom);
DomStatus addParentChild(node_id parentID, node_id childID, Dom *dom);
DomStatus addNextNode(node_id currentNodeID, node_id nextNodeID, Dom *dom);
DomStatus addBooleanProperty(node_id nodeID, element_id propID, Dom *dom);
DomStatus addProperty(node_id nodeID, element_id keyID, element_id valueID,
                      Dom *dom);
DomStatus addTextNode(node_id nodeID, element_id textID, Dom *dom);
DomStatus replaceTextNode(node_id nodeID, element_id newTextID, Dom *dom);
node_id getFirstChild(node_id parentID, const Dom *dom);
node_id getNextNode(node_id currentNodeID, const Dom *dom);

void destroyDom(const Dom *dom);

#endif
