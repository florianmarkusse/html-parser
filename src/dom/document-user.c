
#include <stdlib.h>

#include "dom/document-user.h"
#include "utils/file/read.h"
#include "utils/print/error.h"

DocumentStatus createFromFile(const char *fileLocation, Document *doc) {
    char *buffer = NULL;
    FileStatus fileStatus = readFile(fileLocation, &buffer);
    if (fileStatus != FILE_SUCCESS) {
        ERROR_WITH_CODE_FORMAT(fileStatusToString(fileStatus),
                               "Failed to read file: \"%s\"", fileLocation);
        return DOCUMENT_ERROR_MEMORY;
    }

    DocumentStatus documentStatus = createDocument(buffer, doc);
    if (documentStatus != DOCUMENT_SUCCESS) {
        free(buffer);
        ERROR_WITH_CODE_FORMAT(documentStatusToString(documentStatus),
                               "Failed to create document from file \"%s\"",
                               fileLocation);
        return documentStatus;
    }
    free(buffer);

    return DOCUMENT_SUCCESS;
}
