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

#define FLO_HTML_NODES_PAGE_SIZE (1U << 10U)
#define FLO_HTML_NODES_PER_PAGE                                                \
    (FLO_HTML_NODES_PAGE_SIZE / sizeof(flo_html_Node))

#define FLO_HTML_TAG_REGISTRY_PAGE_SIZE (1U << 8U)
#define FLO_HTML_TAG_REGISTRATIONS_PER_PAGE                                    \
    (FLO_HTML_TAG_REGISTRY_PAGE_SIZE / sizeof(flo_html_TagRegistration))

#define FLO_HTML_PROP_REGISTRY_PAGE_SIZE (1U << 8U)
#define FLO_HTML_PROP_REGISTRATIONS_PER_PAGE                                   \
    (FLO_HTML_PROP_REGISTRY_PAGE_SIZE / sizeof(flo_html_Registration))

#define FLO_HTML_PARENT_FIRST_CHILDS_PAGE_SIZE (1U << 8U)
#define FLO_HTML_PARENT_FIRST_CHILDS_PER_PAGE                                  \
    (FLO_HTML_PARENT_FIRST_CHILDS_PAGE_SIZE / sizeof(flo_html_ParentChild))

#define FLO_HTML_PARENT_CHILDS_PAGE_SIZE (1U << 8U)
#define FLO_HTML_PARENT_CHILDS_PER_PAGE                                        \
    (FLO_HTML_PARENT_CHILDS_PAGE_SIZE / sizeof(flo_html_ParentChild))

#define NEXT_FLO_HTML_NODES_PAGE_SIZE (1U << 8U)
#define NEXT_FLO_HTML_NODES_PER_PAGE                                           \
    (NEXT_FLO_HTML_NODES_PAGE_SIZE / sizeof(flo_html_NextNode))

#define BOOLEAN_PROPERTIES_PAGE_SIZE (1U << 8U)
#define BOOLEAN_PROPERTIES_PER_PAGE                                            \
    (BOOLEAN_PROPERTIES_PAGE_SIZE / sizeof(flo_html_BooleanProperty))

#define PROPERTIES_PAGE_SIZE (1U << 8U)
#define PROPERTIES_PER_PAGE (PROPERTIES_PAGE_SIZE / sizeof(flo_html_Property))

typedef struct {
    flo_html_Registration *registry;
    size_t len;
    size_t cap;
} __attribute__((aligned(32))) flo_html_BasicRegistry;

typedef struct {
    flo_html_node_id firstNodeID;

    flo_html_Node *nodes;
    size_t nodeLen;
    size_t nodeCap;

    flo_html_ParentChild *parentFirstChilds;
    size_t parentFirstChildLen;
    size_t parentFirstChildCap;

    flo_html_ParentChild *parentChilds;
    size_t parentChildLen;
    size_t parentChildCap;

    flo_html_NextNode *nextNodes;
    size_t nextNodeLen;
    size_t nextNodeCap;

    flo_html_BooleanProperty *boolProps;
    size_t boolPropsLen;
    size_t boolPropsCap;

    flo_html_Property *props;
    size_t propsLen;
    size_t propsCap;

    flo_html_TagRegistration *tagRegistry;
    size_t tagRegistryLen;
    size_t tagRegistryCap;

    flo_html_BasicRegistry boolPropRegistry;
    flo_html_BasicRegistry propKeyRegistry;
    flo_html_BasicRegistry propValueRegistry;
} __attribute__((aligned(128))) flo_html_Dom;

/**
 * @brief Create a DOM structure from an HTML string.
 *
 * This function parses the provided `htmlString` and builds the `dom`
 * structure with the parsed DOM representation and populates the
 * `textStore` with all the text values.
 *
 * @param[in]   htmlString      The HTML string to parse.
 * @param[out]  dom             The DOM structure to build.
 * @param[in]   textStore   The text store to populate.
 * values
 *
 * @return  The status of the DOM creation operation (DOM_SUCCESS if completed,
 *          an error code otherwise). See @ref
 *          "flo/html-parser/dom/dom-status.h#flo_html_DomStatus".
 */
flo_html_DomStatus flo_html_createDom(const char *htmlString, flo_html_Dom *dom,
                                      flo_html_TextStore *textStore);

/**
 * @brief Create a DOM structure from an HTML file.
 *
 * This function reads the HTML content from the specified `fileLocation`,
 * parses it, and and builds the `dom` structure with the parsed DOM
 * representation and populates the `textStore` with all the text values.
 *
 * @param[in]   fileLocation    The file location of the HTML content to parse.
 * @param[out]  dom             The DOM structure to build.
 * @param[in]   textStore   The text store to populate.
 *
 * @return  The status of the DOM creation operation (DOM_SUCCESS if completed,
 *          an error code otherwise). See @ref
 *          "flo/html-parser/dom/dom-status.h#flo_html_DomStatus".
 */
flo_html_DomStatus flo_html_createDomFromFile(const char *fileLocation,
                                              flo_html_Dom *dom,
                                              flo_html_TextStore *textStore);

/**
 * @brief Destroy a DOM structure and release associated memory.
 *
 * This function releases the memory associated with the provided `dom`
 * structure.
 *
 * @param[in]   dom     The DOM structure to destroy.
 */
void destroyflo_html_Dom(flo_html_Dom *dom);

#endif
