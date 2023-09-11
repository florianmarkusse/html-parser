#ifndef FLO_HTML_PARSER_DOM_APPENDIX_APPENDIX_H
#define FLO_HTML_PARSER_DOM_APPENDIX_APPENDIX_H

#include <stdbool.h>
#include <stddef.h>

#include "flo/html-parser/dom/dom-status.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/type/node/document-node.h"
#include "flo/html-parser/type/node/node.h"

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
 * @param[in]   dataContainer   The data container.
 *
 * @return  The status of the append operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
DomStatus appendDocumentNodeWithQuery(const char *cssQuery,
                                      const DocumentNode *docNode, Dom *dom,
                                      DataContainer *dataContainer);

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
 * @param[in]   dataContainer   The data container.
 *
 * @return  The status of the append operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
DomStatus appendTextNodeWithQuery(const char *cssQuery, const char *text,
                                  Dom *dom, DataContainer *dataContainer);

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
 * @param[in]   dataContainer   The data container.
 *
 * @return  The status of the append operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
DomStatus appendHTMLFromStringWithQuery(const char *cssQuery,
                                        const char *htmlString, Dom *dom,
                                        DataContainer *dataContainer);

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
 * @param[in]   dataContainer   The data container.
 *
 * @return  The status of the append operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
DomStatus appendHTMLFromFileWithQuery(const char *cssQuery,
                                      const char *fileLocation, Dom *dom,
                                      DataContainer *dataContainer);

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
 * @param[in]   dataContainer   The data container.
 *
 * @return  The status of the append operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
DomStatus appendDocumentNode(node_id parentID, const DocumentNode *docNode,
                             Dom *dom, DataContainer *dataContainer);

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
 * @param[in]   dataContainer   The data container.
 *
 * @return  The status of the append operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
DomStatus appendTextNode(node_id parentID, const char *text, Dom *dom,
                         DataContainer *dataContainer);

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
 * @param[in]   dataContainer   The data container.
 *
 * @return  The status of the append operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
DomStatus appendHTMLFromString(node_id parentID, const char *htmlString,
                               Dom *dom, DataContainer *dataContainer);

#endif
