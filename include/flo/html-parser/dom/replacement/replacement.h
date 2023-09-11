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
 * @param[in]   dataContainer   The data container.
 *
 * @return  The status of the replacement operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
DomStatus replaceWithDocumentNodeWithQuery(const char *cssQuery,
                                           const DocumentNode *docNode,
                                           Dom *dom,
                                           DataContainer *dataContainer);

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
 * @param[in]   dataContainer   The data container.
 *
 * @return  The status of the replacement operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
DomStatus replaceWithTextNodeWithQuery(const char *cssQuery, const char *text,
                                       Dom *dom, DataContainer *dataContainer);

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
 * @param[in]   dataContainer   The data container.
 *
 * @return  The status of the replacement operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
DomStatus replaceWithHTMLFromStringWithQuery(const char *cssQuery,
                                             const char *htmlString, Dom *dom,
                                             DataContainer *dataContainer);

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
 * @param[in]   dataContainer   The data container.
 *
 * @return  The status of the replacement operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
DomStatus replaceWithHTMLFromFileWithQuery(const char *cssQuery,
                                           const char *fileLocation, Dom *dom,
                                           DataContainer *dataContainer);

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
 * @param[in]   dataContainer   The data container.
 *
 * @return  The status of the replacement operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
DomStatus replaceWithDocumentNode(node_id toReplaceNodeID,
                                  const DocumentNode *docNode, Dom *dom,
                                  DataContainer *dataContainer);

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
 * @param[in]   dataContainer   The data container.
 *
 * @return  The status of the replacement operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
DomStatus replaceWithTextNode(node_id toReplaceNodeID, const char *text,
                              Dom *dom, DataContainer *dataContainer);

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
 * @param[in]   dataContainer   The data container.
 *
 * @return  The status of the replacement operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
DomStatus replaceWithHTMLFromString(node_id toReplaceNodeID,
                                    const char *htmlString, Dom *dom,
                                    DataContainer *dataContainer);

#endif
