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
        ERROR_WITH_CODE_FORMAT(fileStatusToString(fileStatus),
                               "Failed to read file: \"%s\"", testFile);
        return 1;
    }
    printf("%s\n", htmlString);

    Document doc;
    DocumentStatus documentStatus = createDocument(htmlString, &doc);
    if (documentStatus != DOCUMENT_SUCCESS) {
        free(htmlString);
        destroyTags();
        ERROR_WITH_CODE_FORMAT(documentStatusToString(documentStatus),
                               "Failed to create document from file \"%s\"",
                               testFile);
        return 1;
    }
    free(htmlString);

    printDocument(&doc);

    destroyDocument(&doc);
    printTagStatus();
    destroyTags();
}
