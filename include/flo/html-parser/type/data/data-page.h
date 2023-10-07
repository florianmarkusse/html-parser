#ifndef FLO_HTML_PARSER_TYPE_DATA_DATA_PAGE_H
#define FLO_HTML_PARSER_TYPE_DATA_DATA_PAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include "data-page-status.h"
#include "flo/html-parser/definitions.h"

typedef struct {
    void *start;
    void *freeSpace;
    flo_html_page_space spaceLeft;
} flo_html_DataPage;

#ifdef __cplusplus
}
#endif

#endif
