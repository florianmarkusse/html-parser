#ifndef FLO_HTML_PARSER_TYPE_DATA_DATA_PAGE_H
#define FLO_HTML_PARSER_TYPE_DATA_DATA_PAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include "flo/html-parser/definitions.h"
#include "flo/html-parser/util/hash/hash-element.h"
#include "flo/html-parser/util/hash/string-hash.h"
#include "flo/html-parser/util/text/string.h"

#define FLO_HTML_EXPONENT 16U
#define FLO_HTML_MAX_ELEMENTS_PER_REGISTRY 1U << 13U
#define FLO_HTML_TOTAL_ELEMENTS (1U << FLO_HTML_EXPONENT)
#define FLO_HTML_POSSIBLE_ELEMENTS (1U << FLO_HTML_EXPONENT)

#define FLO_HTML_TAGS_PAGE_SIZE (1U << 10U)
#define FLO_HTML_BOOL_PROPS_PAGE_SIZE (1U << 12U)
#define FLO_HTML_PROP_KEYS_PAGE_SIZE (1U << 10U)
#define FLO_HTML_PROP_VALUES_PAGE_SIZE (1U << 17U)
#define FLO_HTML_TEXT_PAGE_SIZE (1U << 18U)

typedef struct {
    void *start;
    void *freeSpace;
    flo_html_page_space spaceLeft;
} flo_html_DataPage;

flo_html_DataPage flo_html_initDataPage(const ptrdiff_t pageSize,
                                        flo_html_Arena *perm);

unsigned char *flo_html_insertIntoPage(const flo_html_String data,
                                       flo_html_DataPage *page);

flo_html_index_id flo_html_insertIntoPageWithHash(
    const flo_html_String data, flo_html_DataPage *page,
    flo_html_StringHashSet *set, flo_html_HashElement *hashElement);

#ifdef __cplusplus
}
#endif

#endif
