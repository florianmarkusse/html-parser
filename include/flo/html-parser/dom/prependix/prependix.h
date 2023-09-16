#ifndef FLO_HTML_PARSER_DOM_PREPENDIX_PREPENDIX_H
#define FLO_HTML_PARSER_DOM_PREPENDIX_PREPENDIX_H

#include <stdbool.h>
#include <stddef.h>

#include "flo/html-parser/dom/dom-status.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/type/node/document-node.h"
#include "flo/html-parser/type/node/node.h"

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
 * @param[in]   textStore   The text store.
 *
 * @return  The status of the prepend operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
DomStatus flo_html_prependDocumentNodeWithQuery(const char *cssQuery,
                                       const DocumentNode *docNode, Dom *dom,
                                       TextStore *textStore);

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
 * @param[in]   textStore   The text store.
 *
 * @return  The status of the prepend operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
DomStatus flo_html_prependTextNodeWithQuery(const char *cssQuery, const char *text,
                                   Dom *dom, TextStore *textStore);

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
 * @param[in]   textStore   The text store.
 *
 * @return  The status of the prepend operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
DomStatus flo_html_prependHTMLFromStringWithQuery(const char *cssQuery,
                                         const char *htmlString, Dom *dom,
                                         TextStore *textStore);

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
 * @param[in]   textStore   The text store.
 *
 * @return  The status of the prepend operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
DomStatus flo_html_prependHTMLFromFileWithQuery(const char *cssQuery,
                                       const char *fileLocation, Dom *dom,
                                       TextStore *textStore);

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
 * @param[in]   textStore   The text store.
 *
 * @return  The status of the prepend operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
DomStatus flo_html_prependDocumentNode(node_id parentID, const DocumentNode *docNode,
                              Dom *dom, TextStore *textStore);

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
 * @param[in]   textStore   The text store.
 *
 * @return  The status of the prepend operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
DomStatus flo_html_prependTextNode(node_id parentID, const char *text, Dom *dom,
                          TextStore *textStore);

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
 * @param[in]   textStore   The text store.
 *
 * @return  The status of the prepend operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
DomStatus flo_html_prependHTMLFromString(node_id parentID, const char *htmlString,
                                Dom *dom, TextStore *textStore);

#endif
