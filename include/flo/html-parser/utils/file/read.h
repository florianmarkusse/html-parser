#ifndef FLO_HTML_PARSER_UTILS_FILE_READ_H
#define FLO_HTML_PARSER_UTILS_FILE_READ_H

#include <stddef.h>

#include "file-status.h"

FileStatus readFile(const char *srcPath, char **buffer);

#endif
