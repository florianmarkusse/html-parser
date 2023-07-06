#ifndef DOM_DOCUMENT_STATUS_H
#define DOM_DOCUMENT_STATUS_H

typedef enum {
    DOCUMENT_SUCCESS,
    DOCUMENT_ERROR_MEMORY,
    DOCUMENT_NUM_STATUS
} DocumentStatus;

static const char *const DocumentStatusStrings[DOCUMENT_NUM_STATUS] = {
    "Success",
    "Error: Memory",
};

static const char *documentStatusToString(DocumentStatus status) {
    if (status >= 0 && status < DOCUMENT_NUM_STATUS) {
        return DocumentStatusStrings[status];
    }
    return "Unknown document status code!";
}

#endif
