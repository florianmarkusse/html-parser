#ifndef TYPE_ELEMENT_ELEMENT_STATUS_H
#define TYPE_ELEMENT_ELEMENT_STATUS_H

typedef enum {
    ELEMENT_SUCCESS,
    ELEMENT_NOT_FOUND_OR_CREATED,
    ELEMENT_TOO_LONG,
    ELEMENT_ARRAY_FULL,
    ELEMENT_NUM_STATUS
} ElementStatus;

static const char *const ElementStatusStrings[ELEMENT_NUM_STATUS] = {
    "Success",
    "Element not found or created",
    "Element too long",
    "Element array max size reached",
};

static const char *tagStatusToString(ElementStatus status) {
    if (status >= 0 && status < ELEMENT_NUM_STATUS) {
        return ElementStatusStrings[status];
    }
    return "Unknown element status code!";
}

#endif
