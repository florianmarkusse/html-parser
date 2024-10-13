#ifndef FLO_UTIL_FILE_READ_H
#define FLO_UTIL_FILE_READ_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#include "flo/util/file/file-status.h"
#include "flo/util/memory/arena.h"
#include "flo/util/text/string.h"

flo_FileStatus flo_readFile(char *srcPath, flo_String *buffer, flo_Arena *perm);

#ifdef __cplusplus
}
#endif

#endif
