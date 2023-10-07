#ifndef FLO_HTML_PARSER_NODE_DOCUMENT_NODE_H
#define FLO_HTML_PARSER_NODE_DOCUMENT_NODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

#include "flo/html-parser/util/text/string.h"

#define FLO_HTML_MAX_PROPS_LEN_APPENDIX (1U << 4U)

typedef struct {
    const flo_html_String tag;
    const bool isPaired;
    const flo_html_String boolProps[FLO_HTML_MAX_PROPS_LEN_APPENDIX];
    const ptrdiff_t boolPropsLen;
    const flo_html_String keyProps[FLO_HTML_MAX_PROPS_LEN_APPENDIX];
    const flo_html_String valueProps[FLO_HTML_MAX_PROPS_LEN_APPENDIX];
    const ptrdiff_t propsLen;
} flo_html_DocumentNode;

#ifdef __cplusplus
}
#endif

#endif
