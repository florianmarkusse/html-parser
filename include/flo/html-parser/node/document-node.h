#ifndef FLO_HTML_PARSER_NODE_DOCUMENT_NODE_H
#define FLO_HTML_PARSER_NODE_DOCUMENT_NODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

#include "flo/util/text/string.h"

#define FLO_HTML_MAX_PROPS_LEN_APPENDIX (1U << 4U)

typedef struct {
    flo_String tag;
    bool isPaired;
    flo_String boolProps[FLO_HTML_MAX_PROPS_LEN_APPENDIX];
    ptrdiff_t boolPropsLen;
    flo_String keyProps[FLO_HTML_MAX_PROPS_LEN_APPENDIX];
    flo_String valueProps[FLO_HTML_MAX_PROPS_LEN_APPENDIX];
    ptrdiff_t propsLen;
} flo_html_DocumentNode;

#ifdef __cplusplus
}
#endif

#endif
