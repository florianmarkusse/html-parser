#ifndef DOM_QUERYING_DOCUMENT_QUERYING_STATUS_H
#define DOM_QUERYING_DOCUMENT_QUERYING_STATUS_H

typedef enum {
    QUERYING_SUCCESS,
    QUERYING_INVALID_SELECTOR,
    QUERYING_NOT_FOUND,
    QUERYING_MEMORY_ERROR,
    QUERYING_NUM_STATUS
} QueryingStatus;

static const char *const QueryingStatusStrings[QUERYING_NUM_STATUS] = {
    "Success",
    "Invalid selector",
    "Did not find any match",
    "Memory error",
};

__attribute__((unused)) static const char *
queryingStatusToString(QueryingStatus status) {
    if (status >= 0 && status < QUERYING_NUM_STATUS) {
        return QueryingStatusStrings[status];
    }
    return "Unknown querying status code!";
}

#endif
