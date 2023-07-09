#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "dom/document-writing.h"
#include "dom/document.h"
#include "utils/file/read.h"
#include "utils/print/error.h"

static const char *const testFile = "test/test.html";

int main() {
    createGlobals();

    char *htmlString = NULL;
    FileStatus fileStatus = readFile(testFile, &htmlString);
    if (fileStatus != FILE_SUCCESS) {
        destroyGlobals();
        ERROR_WITH_CODE_FORMAT(fileStatusToString(fileStatus),
                               "Failed to read file: \"%s\"", testFile);
        return 1;
    }
    printf("%s\n", htmlString);

    Document doc;
    DocumentStatus documentStatus = createDocument(htmlString, &doc);
    if (documentStatus != DOCUMENT_SUCCESS) {
        free(htmlString);
        destroyGlobals();
        ERROR_WITH_CODE_FORMAT(documentStatusToString(documentStatus),
                               "Failed to create document from file \"%s\"",
                               testFile);
        return 1;
    }
    free(htmlString);

    printDocumentStatus(&doc);
    printXML(&doc);
    writeXMLToFile(&doc, "test/test-write.html");

    destroyDocument(&doc);
    printElementStatus();
    destroyGlobals();
}
