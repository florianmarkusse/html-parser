#ifndef FLO_HTML_PARSER_TYPE_NODE_REGISTRATION_H
#define FLO_HTML_PARSER_TYPE_NODE_REGISTRATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "flo/html-parser/hash/hash-element.h"
#include "node.h"

typedef struct {
    // flo_html_indexID flo_html_indexID;
    flo_html_HashElement hashElement;
} flo_html_Registration;

#ifdef __cplusplus
}
#endif

#endif
