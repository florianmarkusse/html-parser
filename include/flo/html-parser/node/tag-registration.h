#ifndef FLO_HTML_PARSER_NODE_TAG_REGISTRATION_H
#define FLO_HTML_PARSER_NODE_TAG_REGISTRATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "flo/util/text/string.h"

typedef struct {
    flo_String tag;
    bool isPaired;
} flo_html_TagRegistration;

#ifdef __cplusplus
}
#endif

#endif
