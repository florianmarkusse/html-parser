#include <stdio.h>
#include <stdlib.h>

#include "flo/html-parser/util//file/read.h"
#include "flo/html-parser/util//memory.h"
#include "flo/html-parser/util//error.h"

flo_html_FileStatus flo_html_readFile(const flo_html_String srcPath,
                                      flo_html_String *buffer) {
    // casting to char* here because a srcpath should not contain any weird
    // chars.
    FILE *srcFile = fopen((char *)srcPath.buf, "rbe");
    if (srcFile == NULL) {
        FLO_HTML_PRINT_ERROR("Failed to open source file: %s\n", srcPath.buf);
        return FILE_CANT_OPEN;
    }

    fseek(srcFile, 0, SEEK_END);
    ptrdiff_t dataLen = ftell(srcFile);
    rewind(srcFile);

    (*buffer).buf = (unsigned char *)malloc(dataLen + 1);
    if ((*buffer).buf == NULL) {
        FLO_HTML_PRINT_ERROR("Failed to allocate memory.\n");
        fclose(srcFile);
        return FILE_CANT_ALLOCATE;
    }

    ptrdiff_t result = fread((*buffer).buf, 1, dataLen, srcFile);
    if (result != dataLen) {
        FLO_HTML_PRINT_ERROR("Failed to read the file.\n");
        fclose(srcFile);
        FLO_HTML_FREE_TO_NULL((*buffer).buf);
        return FILE_CANT_READ;
    }

    (*buffer).buf[dataLen] = '\0';
    (*buffer).len = dataLen;

    fclose(srcFile);
    return FILE_SUCCESS;
}
