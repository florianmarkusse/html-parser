#include <stdio.h>
#include <stdlib.h>

#include "utils/file/read.h"

const char *readFile(const char *srcPath, size_t *dataLen) {
    FILE *srcFile = fopen(srcPath, "rbe");
    if (srcFile == NULL) {
        printf("Failed to open source file: %s\n", srcPath);
        return NULL;
    }

    fseek(srcFile, 0, SEEK_END);
    *dataLen = ftell(srcFile);
    rewind(srcFile);

    char *buffer = (char *)malloc(*(dataLen) + 1);
    if (buffer == NULL) {
        printf("Failed to allocate memory.\n");
        fclose(srcFile);
        return NULL;
    }

    size_t result = fread(buffer, 1, *dataLen, srcFile);
    if (result != *dataLen) {
        printf("Failed to read the file.\n");
        fclose(srcFile);
        free(buffer);
        return NULL;
    }

    buffer[*dataLen] = '\0';

    fclose(srcFile);
    return buffer;
}
