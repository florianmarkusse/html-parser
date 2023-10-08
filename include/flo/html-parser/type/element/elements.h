#ifndef FLO_HTML_PARSER_TYPE_ELEMENT_ELEMENTS_H
#define FLO_HTML_PARSER_TYPE_ELEMENT_ELEMENTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include "element-status.h"
#include "flo/html-parser/type/data/data-page.h"
#include "flo/html-parser/util/hash/string-hash.h"
#include "flo/html-parser/util/memory.h"

typedef struct {
    flo_html_DataPage dataPage;
    flo_html_StringHashSet set;
} flo_html_StringRegistry;

void flo_html_initStringRegistry(flo_html_StringRegistry *stringRegistry,
                                 ptrdiff_t stringsCapacity, ptrdiff_t pageSize,
                                 flo_html_Arena *perm);
void flo_html_destroyStringRegistry(flo_html_StringRegistry *stringRegistry);

typedef struct {
    flo_html_StringRegistry tags;
    flo_html_StringRegistry boolProps;
    flo_html_StringRegistry propKeys;
    flo_html_StringRegistry propValues;
    flo_html_DataPage text;
} flo_html_TextStore;

void flo_html_createTextStore(flo_html_TextStore *textStore,
                              flo_html_Arena *perm);
void flo_html_destroyTextStore(flo_html_TextStore *textStore);

flo_html_ElementStatus flo_html_insertElement(flo_html_DataPage *page,
                                              const flo_html_String element,
                                              char **dataLocation);

flo_html_ElementStatus flo_html_elementToIndex(
    flo_html_StringRegistry *stringRegistry, const flo_html_String element,
    flo_html_HashElement *hashElement, flo_html_indexID *flo_html_indexID);

#ifdef __cplusplus
}
#endif

#endif
