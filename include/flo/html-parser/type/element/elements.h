#ifndef FLO_HTML_PARSER_TYPE_ELEMENT_ELEMENTS_H
#define FLO_HTML_PARSER_TYPE_ELEMENT_ELEMENTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include "flo/html-parser/definitions.h"
#include "flo/html-parser/util/hash/hash-element.h"
#include "flo/html-parser/util/hash/string-hash.h"
#include "flo/html-parser/util/memory.h"

typedef flo_html_Contains flo_html_ElementIndex;

#define FLO_HTML_REGISTRY_START_SIZE 1U << 6U

typedef struct {
    flo_html_StringHashSet tags;
    flo_html_StringHashSet boolProps;
    flo_html_StringHashSet propKeys;
    flo_html_StringHashSet propValues;
} flo_html_TextStore;

flo_html_TextStore flo_html_createTextStore(flo_html_Arena *perm);

#ifdef __cplusplus
}
#endif

#endif
