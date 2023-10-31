#ifndef FLO_HTML_PARSER_DOM_WRITING_H
#define FLO_HTML_PARSER_DOM_WRITING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "dom.h"

/**
 * @brief Print the status of a DOM structure.
 *
 * This function prints the status of the provided `dom` structure, including
 * information about node counts, registrations, and other relevant details. It
 * can be useful for debugging and understanding the structure of the parsed
 * DOM.
 *
 * @param[in]   dom             The DOM structure to print status for.
 */
void flo_html_printDomStatus(flo_html_Dom *dom);

/**
 * @brief Print the minified HTML representation of a DOM structure.
 *
 * This function prints the minified HTML representation of the provided `dom`
 * structure, including all the elements, tags, and text content. It is useful
 * for inspecting the parsed HTML document.
 *
 * @param[in]   dom             The DOM structure to print as minified HTML.
 */
void flo_html_printHTML(flo_html_Dom *dom);

/**
 * @brief Write the minified HTML representation of a DOM structure to a file.
 *
 * This function writes the minified HTML representation of the provided `dom`
 * structure to the specified `filePath`. It can be used to save the parsed HTML
 * document to a file.
 *
 * @param[in]   dom             The DOM structure to write as minified HTML.
 * @param[in]   filePath        The file path where the HTML content will be
 *                              saved.
 * @param[in]   scratch            The memory arena for temporary allocations.
 *
 * @return  The status of the file writing operation (FILE_SUCCESS if
 *          completed, an error code otherwise). See @ref
 *          "flo/html-parser/util/file/file-status.h#flo_html_FileStatus".
 */
flo_html_FileStatus flo_html_writeHTMLToFile(flo_html_Dom *dom,
                                             flo_html_String filePath,
                                             flo_html_Arena scratch);

#ifdef __cplusplus
}
#endif

#endif
