#ifndef FLO_HTML_PARSER_DOM_MODIFICATION_H
#define FLO_HTML_PARSER_DOM_MODIFICATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/node/node.h"

/**
 * @brief Add a property with specified key and value to an HTML element.
 *
 * This function adds a property to the HTML element identified by `nodeID`.
 * The property is specified by `key` and `value`.
 *
 * @param[in]   nodeID          The ID of the HTML element to which the property
 *                              will be added.
 * @param[in]   key             The buffer containing the property key.
 * @param[in]   value           The buffer containing the property value.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore       The text store.
 *
 * @return  The status of the property addition (ELEMENT_SUCCESS if successful,
 *          an error code otherwise).
 */
void flo_html_addPropertyToNode(flo_html_node_id nodeID,
                                flo_html_String key,
                                flo_html_String value, flo_html_Dom *dom,
                                flo_html_Arena *perm);

/**
 * @brief Add a boolean property to an HTML element.
 *
 * This function adds a boolean property to the HTML element identified by
 * `nodeID`. The property is specified by `boolProp`.
 *
 * @param[in]   nodeID          The ID of the HTML element to which the boolean
 *                              property will be added.
 * @param[in]   boolProp        The boolean property.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore       The text store.
 *
 * @return  The status of the property addition (ELEMENT_SUCCESS if successful,
 *          an error code otherwise).
 */
void flo_html_addBooleanPropertyToNode(flo_html_node_id nodeID,
                                       flo_html_String boolProp,
                                       flo_html_Dom *dom, flo_html_Arena *perm);

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
bool flo_html_setPropertyValue(flo_html_node_id nodeID,
                               flo_html_String key,
                               flo_html_String newValue,
                               flo_html_Dom *dom, flo_html_Arena *perm);

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
void flo_html_setTextContent(flo_html_node_id nodeID,
                             flo_html_String text, flo_html_Dom *dom,
                             flo_html_Arena *perm);

/**
 * @brief Add text to a text node within an HTML element.
 *
 * This function adds text content to a text node within the HTML element
 * specified by `node`. If `isAppend` is true, the text is appended; otherwise,
 * it prepends to the existing text content.
 *
 * @param[in]   node            The HTML element containing the text node.
 * @param[in]   text            The text you want to add.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore       The text store.
 * @param[in]   isAppend        Flag indicating whether to append or prepend the
 *                              text content.
 *
 * @return  The status of the text addition (ELEMENT_SUCCESS if successful,
 *          an error code otherwise).
 */
void flo_html_addTextToTextNode(flo_html_node_id nodeID,
                                flo_html_String text, flo_html_Dom *dom,
                                bool isAppend, flo_html_Arena *perm);

/**
 * @brief Set the tag on a DocumentNode within the DOM.
 *
 * This function sets the tag for the DocumentNode identified by `nodeID` within
 * the DOM structure. Whether the tag is paired or not is indicated by
 * `isPaired`.
 *
 * @param[in]   tag             The tag.
 * @param[in]   nodeID          The ID of the DocumentNode to update.
 * @param[in]   isPaired        Flag indicating whether the tag is paired or
 *                              not.
 * @param[in]   dom             The DOM structure.
 * @param[in]   textStore       The text store.
 *
 * @return  The status of the tag setting operation (DOM_SUCCESS if successful,
 *          an error code otherwise).
 */
void flo_html_setTagOnDocumentNode(flo_html_String tag,
                                   flo_html_node_id nodeID, bool isPaired,
                                   flo_html_Dom *dom, flo_html_Arena *perm);

#ifdef __cplusplus
}
#endif

#endif
