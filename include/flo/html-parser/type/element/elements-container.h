#ifndef FLO_HTML_PARSER_TYPE_ELEMENT_ELEMENTS_CONTAINER_H
#define FLO_HTML_PARSER_TYPE_ELEMENT_ELEMENTS_CONTAINER_H

#include <stddef.h>
#include <stdint.h>

#include "element-status.h"
#include "flo/html-parser/type/data/data-page.h"

typedef uint16_t flo_html_element_id;
#define FLO_HTML_EXPONENT 16U
#define FLO_HTML_TOTAL_ELEMENTS (1U << FLO_HTML_EXPONENT)
#define FLO_HTML_POSSIBLE_ELEMENTS (1U << FLO_HTML_EXPONENT)

#define FLO_HTML_TOTAL_PAGES (1U << 9U)

// TODO(florian): THIS IS VERY BIG TO START WITH
// The size of each page of each global.
#define FLO_HTML_TAGS_PAGE_SIZE (1U << 12U)
#define FLO_HTML_BOOL_PROPS_PAGE_SIZE (1U << 12U)
#define FLO_HTML_PROP_KEYS_PAGE_SIZE (1U << 12U)
#define FLO_HTML_PROP_VALUES_PAGE_SIZE (1U << 14U)
#define FLO_HTML_TEXT_PAGE_SIZE (1U << 17U)

// TODO(florian): maybe also do smth similar with the pages array instead of
// having fixed pagesize?
typedef struct {
    flo_html_DataPage *pages;
    size_t pageLen;
    size_t pageSize;
} __attribute__((aligned(32))) flo_html_ElementsContainer;

flo_html_ElementStatus
initflo_html_ElementsContainer(flo_html_ElementsContainer *elementsContainer,
                               size_t pageSize);
void destroyflo_html_ElementsContainer(
    flo_html_ElementsContainer *elementsContainer);

#endif
