#ifndef TYPE_DATA_DATA_PAGE_STATUS_H
#define TYPE_DATA_DATA_PAGE_STATUS_H

typedef enum {
    DATA_PAGE_SUCCESS,
    DATA_PAGE_NO_CAPACITY,
    DATA_PAGE_ERROR_MEMORY,
    DATA_PAGE_NUM_STATUS
} DataPageStatus;

static const char *const DataPageStatusStrings[DATA_PAGE_NUM_STATUS] = {
    "Success",
    "No capacity",
    "Memory",
};

static const char *dataPageStatusToString(DataPageStatus status) {
    if (status >= 0 && status < DATA_PAGE_NUM_STATUS) {
        return DataPageStatusStrings[status];
    }
    return "Unknown data page status code!";
}

#endif
