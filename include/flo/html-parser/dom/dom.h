#ifndef FLO_HTML_PARSER_DOM_DOM_H
#define FLO_HTML_PARSER_DOM_DOM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "flo/html-parser/node/boolean-property.h"
#include "flo/html-parser/node/next-node.h"
#include "flo/html-parser/node/node.h"
#include "flo/html-parser/node/parent-child.h"
#include "flo/html-parser/node/property.h"
#include "flo/html-parser/node/tag-registration.h"
#include "flo/html-parser/node/text-node.h"
#include "flo/html-parser/util/array.h"
#include "flo/html-parser/util/file/file-status.h"
#include "flo/html-parser/util/hash/string-hash.h"
#include "flo/html-parser/util/memory.h"

#define FLO_HTML_ERROR_NODE_ID 0U
#define FLO_HTML_ROOT_NODE_ID 1U

#define FLO_HTML_REGISTRY_START_SIZE 1U << 12U

typedef FLO_HTML_DYNAMIC_ARRAY(flo_html_Node) flo_html_Node_d_a;
typedef FLO_HTML_DYNAMIC_ARRAY(flo_html_ParentChild) flo_html_ParentChild_d_a;
typedef FLO_HTML_DYNAMIC_ARRAY(flo_html_NextNode) flo_html_NextNode_d_a;
typedef FLO_HTML_DYNAMIC_ARRAY(flo_html_BooleanProperty)
    flo_html_BooleanProperty_d_a;
typedef FLO_HTML_DYNAMIC_ARRAY(flo_html_Property) flo_html_Property_d_a;
typedef FLO_HTML_DYNAMIC_ARRAY(flo_html_TagRegistration)
    flo_html_TagRegistration_d_a;
typedef FLO_HTML_DYNAMIC_ARRAY(flo_html_HashElement) flo_html_HashElement_d_a;

typedef struct {
    // Data containing the node id and associated attriubtes.
    flo_html_Node_d_a nodes;
    flo_html_ParentChild_d_a parentFirstChilds;
    flo_html_ParentChild_d_a parentChilds;
    flo_html_NextNode_d_a nextNodes;
    flo_html_BooleanProperty_d_a boolProps;
    flo_html_Property_d_a props;

    // Data containing the ID and associated hash
    // TODO: what to do when underlying set has rehashed?
    flo_html_TagRegistration_d_a tagRegistry;
    flo_html_String_d_a boolPropRegistry;
    flo_html_String_d_a propKeyRegistry;
    flo_html_String_d_a propValueRegistry;

    // Data containg the acual string values.
    flo_html_StringHashSet tags;
    flo_html_StringHashSet boolPropsSet;
    flo_html_StringHashSet propKeys;
    flo_html_StringHashSet propValues;
} flo_html_Dom;

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
flo_html_Dom *flo_html_createDom(flo_html_String htmlString,
                                 flo_html_Arena *perm);

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
flo_html_Dom *flo_html_createDomFromFile(flo_html_String fileLocation,
                                         flo_html_Arena *perm);

#ifdef __cplusplus
}
#endif

#endif
