#ifndef FLO_HTML_PARSER_DOM_DOM_H
#define FLO_HTML_PARSER_DOM_DOM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "array.h"
#include "file/file-status.h"
#include "flo/html-parser/node/boolean-property.h"
#include "flo/html-parser/node/next-node.h"
#include "flo/html-parser/node/node.h"
#include "flo/html-parser/node/parent-child.h"
#include "flo/html-parser/node/property.h"
#include "flo/html-parser/node/tag-registration.h"
#include "flo/html-parser/node/text-node.h"
#include "flo/html-parser/util/string-auto-uint16-map.h"
#include "hash/string-hash.h"
#include "memory/arena.h"
#include "types.h"

#define FLO_HTML_ERROR_NODE_ID 0U
#define FLO_HTML_ROOT_NODE_ID 1U

#define FLO_HTML_REGISTRY_START_SIZE 1U << 8U

typedef FLO_DYNAMIC_ARRAY(flo_html_Node) flo_html_Node_d_a;
typedef FLO_DYNAMIC_ARRAY(flo_html_ParentChild) flo_html_ParentChild_d_a;
typedef FLO_DYNAMIC_ARRAY(flo_html_NextNode) flo_html_NextNode_d_a;
typedef FLO_DYNAMIC_ARRAY(flo_html_BooleanProperty)
    flo_html_BooleanProperty_d_a;
typedef FLO_DYNAMIC_ARRAY(flo_html_Property) flo_html_Property_d_a;
typedef FLO_DYNAMIC_ARRAY(flo_html_TagRegistration)
    flo_html_TagRegistration_d_a;

typedef struct {
    // Data containing the node id and associated attriubtes.
    flo_html_Node_d_a nodes;
    flo_html_ParentChild_d_a parentFirstChilds;
    flo_html_ParentChild_d_a parentChilds;
    flo_html_NextNode_d_a nextNodes;
    flo_html_BooleanProperty_d_a boolProps;
    flo_html_Property_d_a props;

    // Data containing the found strings.
    // Map from ID -> String
    flo_html_TagRegistration_d_a tagRegistry;
    flo_String_d_a boolPropRegistry;
    flo_String_d_a propKeyRegistry;
    flo_String_d_a propValueRegistry;

    // Data containg the acual string values.
    // Map from String -> ID
    flo_trie_StringAutoUint16Map tagMap;
    flo_trie_StringAutoUint16Map boolPropMap;
    flo_trie_StringAutoUint16Map propKeyMap;
    flo_trie_StringAutoUint16Map propValueMap;
} flo_html_Dom;

/**
 * @brief Create a DOM structure from an HTML string.
 *
 * This function parses the provided `htmlString` and builds the `dom`
 * structure with the parsed DOM representation.
 *
 * @param[in]   htmlString      The HTML string to parse.
 * @param[in]   perm            The memory arena for permanent allocations.
 *
 * @return  The DOM of the html string, NULL if unsuccessful.
 */
flo_html_Dom *flo_html_createDom(flo_String htmlString, flo_Arena *perm);

/**
 * @brief Create a DOM structure from an HTML file.
 *
 * This function reads the HTML content from the specified `fileLocation`,
 * parses it, and builds the `dom` structure with the parsed DOM
 * representation.
 *
 * @param[in]   fileLocation    The file location of the HTML content to parse.
 * @param[out]  dom             The DOM structure to build.
 *
 * @return  The DOM of the html string, NULL if unsuccessful.
 */
flo_html_Dom *flo_html_createDomFromFile(flo_String fileLocation,
                                         flo_Arena *perm);

/**
 * @brief Create a DOM structure identical to the one provided.
 *
 * This function duplicates all the information that is present in the provided
 * `dom` and creates an idential one.
 *
 * @param[out]  dom             The DOM structure to duplicate.
 * @param[in]   perm            The memory arena for permanent allocations.
 *
 * @return  The DOM of the html string.
 */
flo_html_Dom *flo_html_duplicateDom(flo_html_Dom *dom, flo_Arena *perm);

#ifdef __cplusplus
}
#endif

#endif
