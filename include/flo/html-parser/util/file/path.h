#ifndef FLO_HTML_PARSER_UTIL_FILE_PATH_H
#define FLO_HTML_PARSER_UTIL_FILE_PATH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "flo/html-parser/util/memory.h"
#include "flo/html-parser/util/text/string.h"

void flo_html_createPath(flo_html_String fileLocation,
                         flo_html_Arena scratch);

#ifdef __cplusplus
}
#endif

#endif
