#ifndef FLO_HTML_PARSER_DOM_DOM_H
#define FLO_HTML_PARSER_DOM_DOM_H

#include "dom-status.h"
#include "flo/html-parser/type/node/boolean-property.h"
#include "flo/html-parser/type/node/next-node.h"
#include "flo/html-parser/type/node/node.h"
#include "flo/html-parser/type/node/parent-child.h"
#include "flo/html-parser/type/node/property.h"
#include "flo/html-parser/type/node/registration.h"
#include "flo/html-parser/type/node/tag-registration.h"
#include "flo/html-parser/type/node/text-node.h"
#include "flo/html-parser/utils/file/file-status.h"

#define NODES_PAGE_SIZE (1U << 10U)
#define NODES_PER_PAGE (NODES_PAGE_SIZE / sizeof(Node))

#define TAG_REGISTRY_PAGE_SIZE (1U << 8U)
#define TAG_REGISTRATIONS_PER_PAGE                                             \
    (TAG_REGISTRY_PAGE_SIZE / sizeof(TagRegistration))

#define PROP_REGISTRY_PAGE_SIZE (1U << 8U)
#define PROP_REGISTRATIONS_PER_PAGE                                            \
    (PROP_REGISTRY_PAGE_SIZE / sizeof(Registration))

#define PARENT_FIRST_CHILDS_PAGE_SIZE (1U << 8U)
#define PARENT_FIRST_CHILDS_PER_PAGE                                           \
    (PARENT_FIRST_CHILDS_PAGE_SIZE / sizeof(ParentChild))

#define PARENT_CHILDS_PAGE_SIZE (1U << 8U)
#define PARENT_CHILDS_PER_PAGE (PARENT_CHILDS_PAGE_SIZE / sizeof(ParentChild))

#define NEXT_NODES_PAGE_SIZE (1U << 8U)
#define NEXT_NODES_PER_PAGE (NEXT_NODES_PAGE_SIZE / sizeof(NextNode))

#define BOOLEAN_PROPERTIES_PAGE_SIZE (1U << 8U)
#define BOOLEAN_PROPERTIES_PER_PAGE                                            \
    (BOOLEAN_PROPERTIES_PAGE_SIZE / sizeof(BooleanProperty))

#define PROPERTIES_PAGE_SIZE (1U << 8U)
#define PROPERTIES_PER_PAGE (PROPERTIES_PAGE_SIZE / sizeof(Property))

typedef struct {
    Registration *registry;
    size_t len;
    size_t cap;
} __attribute__((aligned(32))) BasicRegistry;

typedef struct {
    node_id firstNodeID;

    Node *nodes;
    size_t nodeLen;
    size_t nodeCap;

    TagRegistration *tagRegistry;
    size_t tagRegistryLen;
    size_t tagRegistryCap;

    BasicRegistry boolPropRegistry;
    BasicRegistry propKeyRegistry;
    BasicRegistry propValueRegistry;

    ParentChild *parentFirstChilds;
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
} __attribute__((aligned(128))) Dom;

DomStatus createDom(const char *htmlString, Dom *dom,
                    DataContainer *dataContainer);

DomStatus createNode(node_id *nodeID, NodeType nodeType, Dom *dom);
void setNodeTagID(node_id nodeID, indexID tagID, Dom *dom);
void setNodeText(node_id nodeID, const char *text, Dom *dom);

DomStatus addParentFirstChild(node_id parentID, node_id childID, Dom *dom);

DomStatus addParentChild(node_id parentID, node_id childID, Dom *dom);

DomStatus addNextNode(node_id currentNodeID, node_id nextNodeID, Dom *dom);

DomStatus addBooleanProperty(node_id nodeID, element_id propID, Dom *dom);

DomStatus addProperty(node_id nodeID, element_id keyID, element_id valueID,
                      Dom *dom);

void destroyDom(Dom *dom);

#endif
