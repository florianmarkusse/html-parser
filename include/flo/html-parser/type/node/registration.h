#ifndef FLO_HTML_PARSER_TYPE_NODE_REGISTRATION_H
#define FLO_HTML_PARSER_TYPE_NODE_REGISTRATION_H

#include <stdbool.h>
#include <stdint.h>

#include "flo/html-parser/hash/hash-element.h"
#include "node.h"

typedef struct {
    flo_html_indexID flo_html_indexID;
    flo_html_HashElement hashElement;
} __attribute__((aligned(32))) flo_html_Registration;

#endif
