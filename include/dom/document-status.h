#ifndef DOM_DOCUMENT_STATUS_H
#define DOM_DOCUMENT_STATUS_H

typedef enum {
    DOCUMENT_SUCCESS,
    DOCUMENT_ERROR_MEMORY,
    DOCUMENT_NO_ADD,
    DOCUMENT_NO_ELEMENT,
    DOCUMENT_TOO_DEEP,
    DOCUMENT_TOO_MANY_ATTRIBUTES,
    DOCUMENT_NUM_STATUS
} DocumentStatus;

static const char *const DocumentStatusStrings[DOCUMENT_NUM_STATUS] = {
    "Success",
    "Memory",
    "Could not add to document",
    "Could not find element",
    "Too many nested tags",
    "Too many attributes on single tag"};

// Not always used, but very handy for those that actually do want readable
// error codes.
__attribute__((unused)) static const char *
documentStatusToString(DocumentStatus status) {
    if (status >= 0 && status < DOCUMENT_NUM_STATUS) {
        return DocumentStatusStrings[status];
    }
    return "Unknown document status code!";
}

#endif
