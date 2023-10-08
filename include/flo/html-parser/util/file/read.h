#ifndef FLO_HTML_PARSER_UTIL_FILE_READ_H
#define FLO_HTML_PARSER_UTIL_FILE_READ_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#include "file-status.h"
#include "flo/html-parser/util/memory.h"
#include "flo/html-parser/util/text/string.h"

flo_html_FileStatus flo_html_readFile(const flo_html_String srcPath,
                                      flo_html_String *buffer,
                                      flo_html_Arena *perm);

#ifdef __cplusplus
}
#endif

#endif
