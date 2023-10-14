#ifndef FLO_HTML_PARSER_USER_H
#define FLO_HTML_PARSER_USER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/type/element/elements.h"
#include "flo/html-parser/util/text/string.h"
#include "user-status.h"

typedef struct {
    flo_html_TextStore *textStore;
    flo_html_Dom *dom;
} flo_html_ParsedHTML;

flo_html_UserStatus flo_html_fromFile(const flo_html_String fileLocation,
                                      flo_html_ParsedHTML parsed,
                                      flo_html_Arena *perm);

void flo_html_destroyHTML(flo_html_Arena *perm);

#ifdef __cplusplus
}
#endif

#endif
