#ifndef FLO_HTML_PARSER_UTILS_FILE_READ_H
#define FLO_HTML_PARSER_UTILS_FILE_READ_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#include "file-status.h"

flo_html_FileStatus flo_html_readFile(const char *srcPath, char **buffer);

#ifdef __cplusplus
}
#endif

#endif
