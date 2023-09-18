#ifndef FLO_HTML_PARSER_TYPE_ELEMENT_ELEMENTS_H
#define FLO_HTML_PARSER_TYPE_ELEMENT_ELEMENTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include "element-status.h"
#include "elements-container.h"
#include "flo/html-parser/hash/string-hash.h"

typedef struct {
    flo_html_ElementsContainer container;
    flo_html_StringHashSet set;
} __attribute__((aligned(64))) flo_html_StringRegistry;

flo_html_ElementStatus
flo_html_initStringRegistry(flo_html_StringRegistry *stringRegistry,
                            size_t stringsCapacity, size_t pageSize);
void flo_html_destroyStringRegistry(flo_html_StringRegistry *stringRegistry);

typedef struct {
    flo_html_StringRegistry tags;
    flo_html_StringRegistry boolProps;
    flo_html_StringRegistry propKeys;
    flo_html_StringRegistry propValues;
    flo_html_ElementsContainer text;
} __attribute__((aligned(128))) flo_html_TextStore;

flo_html_ElementStatus flo_html_createTextStore(flo_html_TextStore *textStore);
void flo_html_destroyTextStore(flo_html_TextStore *textStore);

flo_html_ElementStatus flo_html_createElement(
    flo_html_ElementsContainer *container, const char *element,
    flo_html_element_id *currentElementsLen, flo_html_element_id offset,
    flo_html_element_id *elementID);

flo_html_ElementStatus
flo_html_insertElement(flo_html_ElementsContainer *elementsContainer,
                       const char *elementStart, size_t elementLength,
                       char **dataLocation);

flo_html_ElementStatus
flo_html_elementToIndex(flo_html_StringRegistry *stringRegistry,
                        const char *elementStart, size_t elementLength,
                        flo_html_HashElement *hashElement,
                        flo_html_indexID *flo_html_indexID);

#ifdef __cplusplus
}
#endif

#endif
