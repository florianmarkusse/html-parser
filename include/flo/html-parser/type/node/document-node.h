#ifndef FLO_HTML_PARSER_TYPE_NODE_DOCUMENT_NODE_H
#define FLO_HTML_PARSER_TYPE_NODE_DOCUMENT_NODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

#define FLO_HTML_MAX_PROPS_LEN_APPENDIX (1U << 4U)

typedef struct {
    const char *tag;
    const bool isPaired;
    const char *boolProps[FLO_HTML_MAX_PROPS_LEN_APPENDIX];
    const size_t boolPropsLen;
    const char *keyProps[FLO_HTML_MAX_PROPS_LEN_APPENDIX];
    const char *valueProps[FLO_HTML_MAX_PROPS_LEN_APPENDIX];
    const size_t propsLen;
} __attribute__((aligned(128))) flo_html_DocumentNode;

#ifdef __cplusplus
}
#endif

#endif
