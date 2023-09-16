#ifndef FLO_HTML_PARSER_TYPE_DATA_DATA_PAGE_STATUS_H
#define FLO_HTML_PARSER_TYPE_DATA_DATA_PAGE_STATUS_H

typedef enum {
    DATA_PAGE_SUCCESS,
    DATA_PAGE_NO_CAPACITY,
    DATA_PAGE_ERROR_MEMORY,
    DATA_PAGE_NUM_STATUS
} flo_html_DataPageStatus;

static const char *const dataPageStatusStrings[DATA_PAGE_NUM_STATUS] = {
    "Success",
    "No capacity",
    "Memory",
};

// Not always used, but very handy for those that actually do want readable
// error codes.
__attribute__((unused)) static const char *
flo_html_dataPageStatusToString(flo_html_DataPageStatus status) {
    if (status >= 0 && status < DATA_PAGE_NUM_STATUS) {
        return dataPageStatusStrings[status];
    }
    return "Unknown data page status code!";
}

#endif
