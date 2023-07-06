#ifndef TYPE_TAG_STATUS_H
#define TYPE_TAG_STATUS_H

typedef enum {
    TAG_SUCCESS,
    TAG_TOO_LONG,
    TAG_ERROR_MEMORY,
    TAG_NO_CAPACITY,
    TAG_NUM_STATUS
} TagStatus;

static const char *const TagStatusStrings[TAG_NUM_STATUS] = {
    "Success",
    "Error: Tag too long",
    "Error: Memory",
    "Error: No capacity",
};

static const char *tagStatusToString(TagStatus status) {
    if (status >= 0 && status < TAG_NUM_STATUS) {
        return TagStatusStrings[status];
    }
    return "Unknown tag status code!";
}

#endif
