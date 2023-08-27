#ifndef FLO_HTML_PARSER_TYPE_NODE_TAG_REGISTRATION_H
#define FLO_HTML_PARSER_TYPE_NODE_TAG_REGISTRATION_H

#include <stdbool.h>
#include <stdint.h>

#include "flo/html-parser/hash/hash-element.h"
#include "node.h"

typedef struct {
    indexID tagID;
    HashElement hashElement;
    bool isPaired;
} __attribute__((aligned(32))) TagRegistration;

#endif