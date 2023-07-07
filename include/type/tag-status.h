#ifndef TYPE_TAG_STATUS_H
#define TYPE_TAG_STATUS_H

typedef enum {
    TAG_SUCCESS,
    TAG_NOT_FOUND_OR_CREATED,
    TAG_TOO_LONG,
    TAG_NUM_STATUS
} TagStatus;

static const char *const TagStatusStrings[TAG_NUM_STATUS] = {
    "Success",
    "Tag not found or created",
    "Tag too long",
};

static const char *tagStatusToString(TagStatus status) {
    if (status >= 0 && status < TAG_NUM_STATUS) {
        return TagStatusStrings[status];
    }
    return "Unknown tag status code!";
}

#endif
