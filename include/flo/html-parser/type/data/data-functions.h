#ifndef FLO_HTML_PARSER_TYPE_DATA_DATA_FUNCTIONS_H
#define FLO_HTML_PARSER_TYPE_DATA_DATA_FUNCTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "flo/html-parser/type/element/elements-container.h"
#include "flo/html-parser/type/element/elements.h"

flo_html_DataPageStatus flo_html_insertIntoSuitablePage(
    const void *data, size_t byteLen, size_t totalPages,
    flo_html_ElementsContainer *container, char **dataLocation);

flo_html_DataPageStatus flo_html_insertIntoPageWithHash(
    const void *data, size_t byteLen, size_t totalPages,
    flo_html_StringRegistry *stringRegistry, flo_html_HashElement *hashElement,
    flo_html_indexID *flo_html_indexID);

#ifdef __cplusplus
}
#endif

#endif
