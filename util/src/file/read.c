#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "flo/util/file/read.h"
#include "flo/util/log.h"
#include "flo/util/memory/arena.h"

flo_FileStatus flo_readFile(char *srcPath, flo_String *buffer,
                            flo_Arena *perm) {
    FILE *srcFile = fopen(srcPath, "rbe");
    if (srcFile == NULL) {
        FLO_FLUSH_AFTER(FLO_STDERR) {
            FLO_ERROR("Failed to open file: ");
            FLO_ERROR(srcPath, FLO_NEWLINE);
        }
        return FILE_CANT_OPEN;
    }

    fseek(srcFile, 0, SEEK_END);
    ptrdiff_t dataLen = ftell(srcFile);
    rewind(srcFile);

    (*buffer).buf = FLO_NEW(perm, unsigned char, dataLen, FLO_NULL_ON_FAIL);
    if ((*buffer).buf == NULL) {
        FLO_FLUSH_AFTER(FLO_STDERR) {
            FLO_ERROR((FLO_STRING("Failed to allocate memory for file ")));
            FLO_ERROR(srcPath, FLO_NEWLINE);
        }
        fclose(srcFile);
        return FILE_CANT_ALLOCATE;
    }

    ptrdiff_t result = fread((*buffer).buf, 1, dataLen, srcFile);
    if (result != dataLen) {
        FLO_FLUSH_AFTER(FLO_STDERR) {
            FLO_ERROR((FLO_STRING("Failed to read the file contents of ")));
            FLO_ERROR(srcPath, FLO_NEWLINE);
        }
        fclose(srcFile);
        return FILE_CANT_READ;
    }

    (*buffer).len = dataLen;

    fclose(srcFile);
    return FILE_SUCCESS;
}
