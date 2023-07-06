#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "dom/document.h"
#include "utils/file/read.h"
#include "utils/print/error.h"

static const char *const testFile = "test/test.html";

int main() {
    createTags();

    char *htmlString = NULL;
    FileStatus fileStatus = readFile(testFile, &htmlString);
    if (fileStatus != FILE_SUCCESS) {
        destroyTags();
        PRINT_ERROR("Failed to read file:\t%s", testFile);
        PRINT_ERROR("Recevied status code:\t%s",
                    fileStatusToString(fileStatus));
        return 1;
    }
    printf("%s\n", htmlString);

    Document doc;
    DocumentStatus documentStatus = createDocument(htmlString, &doc);
    if (documentStatus != DOCUMENT_SUCCESS) {
        free(htmlString);
        destroyTags();
        PRINT_ERROR("Failed to create document from file:\t%s", testFile);
        PRINT_ERROR("Recevied status code:\t%s",
                    documentStatusToString(documentStatus));
        return 1;
    }

    printDocument(&doc);

    destroyDocument(&doc);
    printTagStatus();
    destroyTags();
}
