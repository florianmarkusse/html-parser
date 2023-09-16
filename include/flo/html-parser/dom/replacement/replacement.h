#ifndef FLO_HTML_PARSER_DOM_REPLACEMENT_REPLACEMENT_H
#define FLO_HTML_PARSER_DOM_REPLACEMENT_REPLACEMENT_H

#include <stdbool.h>
#include <stddef.h>

#include "flo/html-parser/dom/dom-status.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/type/node/document-node.h"
#include "flo/html-parser/type/node/node.h"

/**
 * @brief Replace an HTML element with a DocumentNode using a CSS query.
 *
 * This function replaces an HTML element specified by `cssQuery` with a
 * `DocumentNode` specified by `docNode`. The operation modifies the DOM
 * structure in place.
 *
 * @param[in]   cssQuery        The CSS query to select the element to replace.
 * @param[in]   docNode         The `DocumentNode` to replace with.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore   The text store.
 *
 * @return  The status of the replacement operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
flo_html_DomStatus flo_html_replaceWithDocumentNodeWithQuery(
    const char *cssQuery, const flo_html_DocumentNode *docNode,
    flo_html_Dom *dom, flo_html_TextStore *textStore);

/**
 * @brief Replace an HTML element with a text node using a CSS query.
 *
 * This function replaces an HTML element specified by `cssQuery` with a text
 * node containing the specified `text`. The operation modifies the DOM
 * structure in place.
 *
 * @param[in]   cssQuery        The CSS query to select the element to replace.
 * @param[in]   text            The text content for the replacement text node.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore   The text store.
 *
 * @return  The status of the replacement operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
flo_html_DomStatus
flo_html_replaceWithTextNodeWithQuery(const char *cssQuery, const char *text,
                                      flo_html_Dom *dom,
                                      flo_html_TextStore *textStore);

/**
 * @brief Replace an HTML element with HTML content from a string using a CSS
 * query.
 *
 * This function replaces an HTML element specified by `cssQuery` with HTML
 * content specified by `htmlString`. The operation modifies the DOM structure
 * in place.
 *
 * @param[in]   cssQuery        The CSS query to select the element to replace.
 * @param[in]   htmlString      The HTML content as a string for replacement.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore   The text store.
 *
 * @return  The status of the replacement operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
flo_html_DomStatus flo_html_replaceWithHTMLFromStringWithQuery(
    const char *cssQuery, const char *htmlString, flo_html_Dom *dom,
    flo_html_TextStore *textStore);

/**
 * @brief Replace an HTML element with HTML content from a file using a CSS
 * query.
 *
 * This function replaces an HTML element specified by `cssQuery` with HTML
 * content from the specified `fileLocation`. The operation modifies the DOM
 * structure in place.
 *
 * @param[in]   cssQuery        The CSS query to select the element to replace.
 * @param[in]   fileLocation    The file location of the HTML content for
 *                              replacement.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore   The text store.
 *
 * @return  The status of the replacement operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
flo_html_DomStatus flo_html_replaceWithHTMLFromFileWithQuery(
    const char *cssQuery, const char *fileLocation, flo_html_Dom *dom,
    flo_html_TextStore *textStore);

/**
 * @brief Replace an HTML element with a DocumentNode.
 *
 * This function replaces an HTML element specified by `toReplaceNodeID` with a
 * `DocumentNode` specified by `docNode`. The operation modifies the DOM
 * structure in place.
 *
 * @param[in]   toReplaceNodeID The ID of the HTML element to replace.
 * @param[in]   docNode         The `DocumentNode` to replace with.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore   The text store.
 *
 * @return  The status of the replacement operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
flo_html_DomStatus flo_html_replaceWithDocumentNode(
    flo_html_node_id toReplaceNodeID, const flo_html_DocumentNode *docNode,
    flo_html_Dom *dom, flo_html_TextStore *textStore);

/**
 * @brief Replace an HTML element with a text node.
 *
 * This function replaces an HTML element specified by `toReplaceNodeID` with a
 * text node containing the specified `text`. The operation modifies the DOM
 * structure in place.
 *
 * @param[in]   toReplaceNodeID The ID of the HTML element to replace.
 * @param[in]   text            The text content for the replacement text node.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore   The text store.
 *
 * @return  The status of the replacement operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
flo_html_DomStatus
flo_html_replaceWithTextNode(flo_html_node_id toReplaceNodeID, const char *text,
                             flo_html_Dom *dom, flo_html_TextStore *textStore);

/**
 * @brief Replace an HTML element with HTML content from a string.
 *
 * This function replaces an HTML element specified by `toReplaceNodeID` with
 * HTML content specified by `htmlString`. The operation modifies the DOM
 * structure in place.
 *
 * @param[in]   toReplaceNodeID The ID of the HTML element to replace.
 * @param[in]   htmlString      The HTML content as a string for replacement.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore   The text store.
 *
 * @return  The status of the replacement operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
flo_html_DomStatus
flo_html_replaceWithHTMLFromString(flo_html_node_id toReplaceNodeID,
                                   const char *htmlString, flo_html_Dom *dom,
                                   flo_html_TextStore *textStore);

#endif
