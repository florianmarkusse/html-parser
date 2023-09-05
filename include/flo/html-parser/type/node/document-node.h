#ifndef FLO_HTML_PARSER_TYPE_NODE_DOCUMENT_NODE_H
#define FLO_HTML_PARSER_TYPE_NODE_DOCUMENT_NODE_H

#include <stdbool.h>
#include <stddef.h>

#define MAX_PROPS_LEN_APPENDIX (1U << 4U)

typedef struct {
    const char *tag;
    const bool isPaired;
    const char *boolProps[MAX_PROPS_LEN_APPENDIX];
    const size_t boolPropsLen;
    const char *keyProps[MAX_PROPS_LEN_APPENDIX];
    const char *valueProps[MAX_PROPS_LEN_APPENDIX];
    const size_t propsLen;
} __attribute__((aligned(128))) DocumentNode;

#endif
