#ifndef FLO_HTML_PARSER_DOM_MODIFICATION_MODIFICATION_H
#define FLO_HTML_PARSER_DOM_MODIFICATION_MODIFICATION_H

#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/type/element/element-status.h"
#include "flo/html-parser/type/element/elements.h"
#include "flo/html-parser/type/node/node.h"

/**
 * @brief Add a property with specified key and value to an HTML element.
 *
 * This function adds a property to the HTML element identified by `nodeID`.
 * The property is specified by `keyBuffer` and `valueBuffer`. The lengths of
 * `keyBuffer` and `valueBuffer` are given by `keyLen` and `valueLen`,
 * respectively.
 *
 * @param[in]   nodeID          The ID of the HTML element to which the property
 *                              will be added.
 * @param[in]   keyBuffer       The buffer containing the property key.
 * @param[in]   keyLen          The length of the property key.
 * @param[in]   valueBuffer     The buffer containing the property value.
 * @param[in]   valueLen        The length of the property value.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore       The text store.
 *
 * @return  The status of the property addition (ELEMENT_SUCCESS if successful,
 *          an error code otherwise).
 */
flo_html_ElementStatus flo_html_addPropertyToNodeStringsWithLength(
    flo_html_node_id nodeID, const char *keyBuffer, size_t keyLen,
    const char *valueBuffer, size_t valueLen, flo_html_Dom *dom,
    flo_html_TextStore *textStore);

/**
 * @brief Add a property with specified key and value to an HTML element.
 *
 * This function adds a property to the HTML element identified by `nodeID`.
 * The property is specified by `keyBuffer` and `valueBuffer`.
 *
 * @param[in]   nodeID          The ID of the HTML element to which the property
 *                              will be added.
 * @param[in]   keyBuffer       The buffer containing the property key.
 * @param[in]   valueBuffer     The buffer containing the property value.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore       The text store.
 *
 * @return  The status of the property addition (ELEMENT_SUCCESS if successful,
 *          an error code otherwise).
 */
flo_html_ElementStatus flo_html_addPropertyToNodeStrings(
    flo_html_node_id nodeID, const char *keyBuffer, const char *valueBuffer,
    flo_html_Dom *dom, flo_html_TextStore *textStore);

/**
 * @brief Add a boolean property to an HTML element with specified length.
 *
 * This function adds a boolean property to the HTML element identified by
 * `nodeID`. The property is specified by `boolPropBuffer`, with a length of
 * `boolPropLen`.
 *
 * @param[in]   nodeID          The ID of the HTML element to which the boolean
 *                              property will be added.
 * @param[in]   boolPropBuffer  The buffer containing the boolean property.
 * @param[in]   boolPropLen     The length of the boolean property.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore       The text store.
 *
 * @return  The status of the property addition (ELEMENT_SUCCESS if successful,
 *          an error code otherwise).
 */
flo_html_ElementStatus flo_html_addBooleanPropertyToNodeStringWithLength(
    flo_html_node_id nodeID, const char *boolPropBuffer, size_t boolPropLen,
    flo_html_Dom *dom, flo_html_TextStore *textStore);

/**
 * @brief Add a boolean property to an HTML element.
 *
 * This function adds a boolean property to the HTML element identified by
 * `nodeID`. The property is specified by `boolPropBuffer`.
 *
 * @param[in]   nodeID          The ID of the HTML element to which the boolean
 *                              property will be added.
 * @param[in]   boolPropBuffer  The buffer containing the boolean property.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore       The text store.
 *
 * @return  The status of the property addition (ELEMENT_SUCCESS if successful,
 *          an error code otherwise).
 */
flo_html_ElementStatus flo_html_addBooleanPropertyToNodeString(
    flo_html_node_id nodeID, const char *boolPropBuffer, flo_html_Dom *dom,
    flo_html_TextStore *textStore);

/**
 * @brief Set the value of an HTML element's property.
 *
 * This function sets the value of the property with the specified `key` to the
 * new value `newValue` for the HTML element identified by `nodeID`.
 *
 * @param[in]   nodeID          The ID of the HTML element whose property will
 *                              be updated.
 * @param[in]   key             The key of the property to update.
 * @param[in]   newValue        The new value of the property.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore       The text store.
 *
 * @return  The status of the property update (ELEMENT_SUCCESS if successful,
 *          an error code otherwise).
 */
flo_html_ElementStatus flo_html_setPropertyValue(flo_html_node_id nodeID,
                                                 const char *key,
                                                 const char *newValue,
                                                 flo_html_Dom *dom,
                                                 flo_html_TextStore *textStore);

/**
 * @brief Set the text content of an HTML element.
 *
 * This function sets the text content of the HTML element identified by
 * `nodeID` to the specified `text`. The operation modifies the DOM structure in
 * place.
 *
 * @param[in]   nodeID          The ID of the HTML element to update.
 * @param[in]   text            The new text content.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore       The text store.
 *
 * @return  The status of the text content update (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
flo_html_DomStatus flo_html_setTextContent(flo_html_node_id nodeID,
                                           const char *text, flo_html_Dom *dom,
                                           flo_html_TextStore *textStore);

/**
 * @brief Add text to a text node within an HTML element.
 *
 * This function adds text content to a text node within the HTML element
 * specified by `node`. The text content is specified by `textStart` and
 * `textLen`. If `isAppend` is true, the text is appended; otherwise, it
 * prepends to the existing text content.
 *
 * @param[in]   node            The HTML element containing the text node.
 * @param[in]   textStart       A pointer to the start of the text content.
 * @param[in]   textLen         The length of the text content.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore       The text store.
 * @param[in]   isAppend        Flag indicating whether to append or prepend the
 *                              text content.
 *
 * @return  The status of the text addition (ELEMENT_SUCCESS if successful,
 *          an error code otherwise).
 */
flo_html_ElementStatus
flo_html_addTextToTextNode(flo_html_Node *node, const char *textStart,
                           size_t textLen, flo_html_Dom *dom,
                           flo_html_TextStore *textStore, bool isAppend);

/**
 * @brief Set the tag on a DocumentNode within the DOM.
 *
 * This function sets the tag for the DocumentNode identified by `nodeID` within
 * the DOM structure. The tag information is specified by `tagStart` and
 * `tagLen`, and whether the tag is paired or not is indicated by
 * `isPaired`.
 *
 * @param[in]   tagStart        A pointer to the start of the tag information.
 * @param[in]   tagLen          The length of the tag.
 * @param[in]   nodeID          The ID of the DocumentNode to update.
 * @param[in]   isPaired        Flag indicating whether the tag is paired or
 *                              not.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore       The text store.
 *
 * @return  The status of the tag setting operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
flo_html_DomStatus
flo_html_setTagOnDocumentNode(const char *tagStart, size_t tagLen,
                              flo_html_node_id nodeID, bool isPaired,
                              flo_html_Dom *dom, flo_html_TextStore *textStore);

#endif
