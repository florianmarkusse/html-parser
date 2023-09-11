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

/**
 * @brief Create a DOM structure from an HTML string.
 *
 * This function parses the provided `htmlString` and builds the `dom`
 * structure with the parsed DOM representation and populates the
 * `dataContainer` with all the text values.
 *
 * @param[in]   htmlString      The HTML string to parse.
 * @param[out]  dom             The DOM structure to build.
 * @param[in]   dataContainer   The data container to populate.
 * values
 *
 * @return  The status of the DOM creation operation (DOM_SUCCESS if completed,
 *          an error code otherwise). See @ref
 *          "flo/html-parser/dom/dom-status.h#DomStatus".
 */
DomStatus createDom(const char *htmlString, Dom *dom,
                    DataContainer *dataContainer);

/**
 * @brief Create a DOM structure from an HTML file.
 *
 * This function reads the HTML content from the specified `fileLocation`,
 * parses it, and and builds the `dom` structure with the parsed DOM
 * representation and populates the `dataContainer` with all the text values.
 *
 * @param[in]   fileLocation    The file location of the HTML content to parse.
 * @param[out]  dom             The DOM structure to build.
 * @param[in]   dataContainer   The data container to populate.
 *
 * @return  The status of the DOM creation operation (DOM_SUCCESS if completed,
 *          an error code otherwise). See @ref
 *          "flo/html-parser/dom/dom-status.h#DomStatus".
 */
DomStatus createDomFromFile(const char *fileLocation, Dom *dom,
                            DataContainer *dataContainer);

/**
 * @brief Destroy a DOM structure and release associated memory.
 *
 * This function releases the memory associated with the provided `dom`
 * structure.
 *
 * @param[in]   dom     The DOM structure to destroy.
 */
void destroyDom(Dom *dom);

#endif
