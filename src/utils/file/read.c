#include <stdio.h>
#include <stdlib.h>

#include "utils/file/read.h"

FileStatus readFile(const char *srcPath, char **buffer) {
    FILE *srcFile = fopen(srcPath, "rbe");
    if (srcFile == NULL) {
        printf("Failed to open source file: %s\n", srcPath);
        return FILE_CANT_OPEN;
    }

    fseek(srcFile, 0, SEEK_END);
    size_t dataLen = ftell(srcFile);
    rewind(srcFile);

    *buffer = (char *)malloc(dataLen + 1);
    if (*buffer == NULL) {
        printf("Failed to allocate memory.\n");
        fclose(srcFile);
        return FILE_CANT_ALLOCATE;
    }

    size_t result = fread(*buffer, 1, dataLen, srcFile);
    if (result != dataLen) {
        printf("Failed to read the file.\n");
        fclose(srcFile);
        free(*buffer);
        return FILE_CANT_READ;
    }

    (*buffer)[dataLen] = '\0';

    fclose(srcFile);
    return FILE_SUCCESS;
}
