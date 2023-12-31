#ifndef FLO_HTML_PARSER_DOM_PREPENDIX_H
#define FLO_HTML_PARSER_DOM_PREPENDIX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/node/document-node.h"
#include "flo/html-parser/node/node.h"

/**
 * @brief Prepend a DocumentNode to the DOM using a CSS query.
 *
 * This function prepends a `DocumentNode` specified by `docNode` to the DOM
 * using the provided CSS query `cssQuery`. The operation modifies the DOM
 * structure in place.
 *
 * @param[in]   cssQuery        The CSS query to select the insertion point.
 * @param[in]   docNode         The `DocumentNode` to prepend.
 * @param[in]   dom             The DOM structure.
 * @param[in]   perm            The memory arena for permanent allocations.
 *
 * @return  The ID of the prepended DocumentNode, or 0 if the operation failed.
 */
flo_html_node_id
flo_html_prependDocumentNodeWithQuery(flo_String cssQuery,
                                      flo_html_DocumentNode *docNode,
                                      flo_html_Dom *dom, flo_Arena *perm);

/**
 * @brief Prepend a text node to the DOM using a CSS query.
 *
 * This function prepends a text node with the specified `text` to the DOM
 * using the provided CSS query `cssQuery`. The operation modifies the DOM
 * structure in place.
 *
 * @param[in]   cssQuery        The CSS query to select the insertion point.
 * @param[in]   text            The text content to prepend.
 * @param[in]   dom             The DOM structure.
 * @param[in]   perm            The memory arena for permanent allocations.
 *
 * @return  The ID of the prepended text node, or 0 if the operation failed.
 */
flo_html_node_id flo_html_prependTextNodeWithQuery(flo_String cssQuery,
                                                   flo_String text,
                                                   flo_html_Dom *dom,
                                                   flo_Arena *perm);

/**
 * @brief Prepend HTML content from a string to the DOM using a CSS query.
 *
 * This function prepends HTML content specified by `htmlString` to the DOM
 * using the provided CSS query `cssQuery`. The operation modifies the DOM
 * structure in place.
 *
 * @param[in]   cssQuery        The CSS query to select the insertion point.
 * @param[in]   htmlString      The HTML content as a string to prepend.
 * @param[in]   dom             The DOM structure.
 * @param[in]   perm            The memory arena for permanent allocations.
 *
 * @return  The ID of the first prepended node, or 0 if the operation failed.
 */
flo_html_node_id flo_html_prependHTMLFromStringWithQuery(flo_String cssQuery,
                                                         flo_String htmlString,
                                                         flo_html_Dom *dom,
                                                         flo_Arena *perm);

/**
 * @brief Prepend HTML content from a file to the DOM using a CSS query.
 *
 * This function prepends HTML content from the specified `fileLocation` to the
 * DOM using the provided CSS query `cssQuery`. The operation modifies the DOM
 * structure in place.
 *
 * @param[in]   cssQuery        The CSS query to select the insertion point.
 * @param[in]   fileLocation    The file location of the HTML content to
 *                              prepend.
 * @param[in]   dom             The DOM structure.
 * @param[in]   perm            The memory arena for permanent allocations.
 *
 * @return  The ID of the first prepended node, or 0 if the operation failed.
 */
flo_html_node_id flo_html_prependHTMLFromFileWithQuery(flo_String cssQuery,
                                                       char *fileLocation,
                                                       flo_html_Dom *dom,
                                                       flo_Arena *perm);

/**
 * @brief Prepend a DocumentNode to the DOM.
 *
 * This function prepends a `DocumentNode` specified by `docNode` to the DOM.
 * The operation modifies the DOM structure in place.
 *
 * @param[in]   parentID        The ID of the parent node where the
 *                              `DocumentNode` will be prepended.
 * @param[in]   docNode         The `DocumentNode` to prepend.
 * @param[in]   dom             The DOM structure.
 * @param[in]   perm            The memory arena for permanent allocations.
 *
 * @return  The ID of the prepended DocumentNode, or 0 if the operation failed.
 */
flo_html_node_id flo_html_prependDocumentNode(flo_html_node_id parentID,
                                              flo_html_DocumentNode *docNode,
                                              flo_html_Dom *dom,
                                              flo_Arena *perm);

/**
 * @brief Prepend a text node to the DOM.
 *
 * This function prepends a text node with the specified `text` to the DOM.
 * The operation modifies the DOM structure in place.
 *
 * @param[in]   parentID        The ID of the parent node where the text node
 *                              will be prepended.
 * @param[in]   text            The text content to prepend.
 * @param[in]   dom             The DOM structure.
 * @param[in]   perm            The memory arena for permanent allocations.
 *
 * @return  The ID of the prepended text node, or 0 if the operation failed.
 */
flo_html_node_id flo_html_prependTextNode(flo_html_node_id parentID,
                                          flo_String text, flo_html_Dom *dom,
                                          flo_Arena *perm);

/**
 * @brief Prepend HTML content from a string to the DOM.
 *
 * This function prepends HTML content specified by `htmlString` to the DOM.
 * The operation modifies the DOM structure in place.
 *
 * @param[in]   parentID        The ID of the parent node where the HTML nodes
 *                              will be prepended.
 * @param[in]   htmlString      The HTML content as a string to prepend.
 * @param[in]   dom             The DOM structure.
 * @param[in]   perm            The memory arena for permanent allocations.
 *
 * @return  The ID of the first prepended node, or 0 if the operation failed.
 */
flo_html_node_id flo_html_prependHTMLFromString(flo_html_node_id parentID,
                                                flo_String htmlString,
                                                flo_html_Dom *dom,
                                                flo_Arena *perm);

#ifdef __cplusplus
}
#endif

#endif
