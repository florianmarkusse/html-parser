#ifndef FLO_UTIL_FILE_PATH_H
#define FLO_UTIL_FILE_PATH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "flo/util/memory/arena.h"
#include "flo/util/text/string.h"

void flo_createPath(flo_String fileLocation, flo_Arena scratch);

#ifdef __cplusplus
}
#endif

#endif
