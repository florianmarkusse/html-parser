#ifndef FLO_HTML_PARSER_TYPE_ELEMENT_ELEMENTS_PRINT_H
#define FLO_HTML_PARSER_TYPE_ELEMENT_ELEMENTS_PRINT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "elements.h"

void flo_html_printPropertyStatus(const flo_html_TextStore *textStore);
void flo_html_printBoolPropStatus(const flo_html_TextStore *textStore);
void flo_html_printTagStatus(const flo_html_TextStore *textStore);

#ifdef __cplusplus
}
#endif

#endif
