
#include <stdlib.h>

#include "flo/html-parser/dom/dom-user.h"
#include "flo/html-parser/utils/file/read.h"
#include "flo/html-parser/utils/print/error.h"

DomStatus createFromFile(const char *fileLocation, Dom *dom,
                         DataContainer *dataContainer) {
    char *buffer = NULL;
    FileStatus fileStatus = readFile(fileLocation, &buffer);
    if (fileStatus != FILE_SUCCESS) {
        ERROR_WITH_CODE_FORMAT(fileStatusToString(fileStatus),
                               "Failed to read file: \"%s\"", fileLocation);
        return DOM_ERROR_MEMORY;
    }

    DomStatus documentStatus = createDom(buffer, dom, dataContainer);
    if (documentStatus != DOM_SUCCESS) {
        free(buffer);
        ERROR_WITH_CODE_FORMAT(documentStatusToString(documentStatus),
                               "Failed to create document from file \"%s\"",
                               fileLocation);
        return documentStatus;
    }
    free(buffer);

    return DOM_SUCCESS;
}