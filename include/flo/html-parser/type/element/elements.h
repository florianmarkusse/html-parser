#ifndef FLO_HTML_PARSER_TYPE_ELEMENT_ELEMENTS_H
#define FLO_HTML_PARSER_TYPE_ELEMENT_ELEMENTS_H

#include "flo/html-parser/definitions.h"
#include "flo/html-parser/util/hash/hash-element.h"
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

typedef flo_html_Contains flo_html_ElementIndex;

flo_html_StringRegistry flo_html_initStringRegistry(ptrdiff_t stringsCapacity,
                                                    ptrdiff_t pageSize,
                                                    flo_html_Arena *perm);

typedef struct {
    flo_html_StringRegistry tags;
    flo_html_StringRegistry boolProps;
    flo_html_StringRegistry propKeys;
    flo_html_StringRegistry propValues;
    flo_html_DataPage text;
} flo_html_TextStore;

flo_html_TextStore flo_html_createTextStore(flo_html_Arena *perm);

#ifdef __cplusplus
}
#endif

#endif
