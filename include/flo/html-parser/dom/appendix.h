#ifndef FLO_HTML_PARSER_DOM_APPENDIX_H
#define FLO_HTML_PARSER_DOM_APPENDIX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/node/document-node.h"
#include "flo/html-parser/node/node.h"

/**
 * @brief Append a DocumentNode to the DOM using a CSS query.
 *
 * This function appends a `DocumentNode` specified by `docNode` to the DOM
 * using the provided CSS query `cssQuery`. The operation modifies the DOM
 * structure in place.
 *
 * @param[in]   cssQuery        The CSS query to select the insertion point.
 * @param[in]   docNode         The `DocumentNode` to append.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore   The text store.
 *
 * @return  The status of the append operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
flo_html_node_id
flo_html_appendDocumentNodeWithQuery(flo_html_String cssQuery,
                                     flo_html_DocumentNode *docNode,
                                     flo_html_Dom *dom, flo_html_Arena *perm);

/**
 * @brief Append a text node to the DOM using a CSS query.
 *
 * This function appends a text node with the specified `text` to the DOM
 * using the provided CSS query `cssQuery`. The operation modifies the DOM
 * structure in place.
 *
 * @param[in]   cssQuery        The CSS query to select the insertion point.
 * @param[in]   text            The text content to append.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore   The text store.
 *
 * @return  The status of the append operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
flo_html_node_id
flo_html_appendTextNodeWithQuery(flo_html_String cssQuery,
                                 flo_html_String text, flo_html_Dom *dom,
                                 flo_html_Arena *perm);

/**
 * @brief Append HTML content from a string to the DOM using a CSS query.
 *
 * This function appends HTML content specified by `htmlString` to the DOM
 * using the provided CSS query `cssQuery`. The operation modifies the DOM
 * structure in place.
 *
 * @param[in]   cssQuery        The CSS query to select the insertion point.
 * @param[in]   htmlString      The HTML content as a string to append.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore   The text store.
 *
 * @return  The status of the append operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
flo_html_node_id
flo_html_appendHTMLFromStringWithQuery(flo_html_String cssQuery,
                                       flo_html_String htmlString,
                                       flo_html_Dom *dom, flo_html_Arena *perm);

/**
 * @brief Append HTML content from a file to the DOM using a CSS query.
 *
 * This function appends HTML content from the specified `fileLocation` to the
 * DOM using the provided CSS query `cssQuery`. The operation modifies the DOM
 * structure in place.
 *
 * @param[in]   cssQuery        The CSS query to select the insertion point.
 * @param[in]   fileLocation    The file location of the HTML content to append.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore   The text store.
 *
 * @return  The status of the append operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
flo_html_node_id
flo_html_appendHTMLFromFileWithQuery(flo_html_String cssQuery,
                                     flo_html_String fileLocation,
                                     flo_html_Dom *dom, flo_html_Arena *perm);

/**
 * @brief Append a DocumentNode to the DOM.
 *
 * This function appends a `DocumentNode` specified by `docNode` to the DOM.
 * The operation modifies the DOM structure in place.
 *
 * @param[in]   parentID        The ID of the parent node where the
 *                              `DocumentNode` will be appended.
 * @param[in]   docNode         The `DocumentNode` to append.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore   The text store.
 *
 * @return  The status of the append operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
flo_html_node_id
flo_html_appendDocumentNode(flo_html_node_id parentID,
                            flo_html_DocumentNode *docNode,
                            flo_html_Dom *dom, flo_html_Arena *perm);

/**
 * @brief Append a text node to the DOM.
 *
 * This function appends a text node with the specified `text` to the DOM.
 * The operation modifies the DOM structure in place.
 *
 * @param[in]   parentID        The ID of the parent node where the text node
 *                              will be appended.
 * @param[in]   text            The text content to append.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore   The text store.
 *
 * @return  The status of the append operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
flo_html_node_id flo_html_appendTextNode(flo_html_node_id parentID,
                                         flo_html_String text,
                                         flo_html_Dom *dom,
                                         flo_html_Arena *perm);

/**
 * @brief Append HTML content from a string to the DOM.
 *
 * This function appends HTML content specified by `htmlString` to the DOM.
 * The operation modifies the DOM structure in place.
 *
 * @param[in]   parentID        The ID of the parent node where the HTML nodes
 *                              will be appended.
 * @param[in]   htmlString      The HTML content as a string to append.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore   The text store.
 *
 * @return  The status of the append operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
flo_html_node_id flo_html_appendHTMLFromString(flo_html_node_id parentID,
                                               flo_html_String htmlString,
                                               flo_html_Dom *dom,
                                               flo_html_Arena *perm);

#ifdef __cplusplus
}
#endif

#endif
