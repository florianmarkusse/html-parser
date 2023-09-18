#ifndef FLO_HTML_PARSER_TYPE_ELEMENT_ELEMENT_STATUS_H
#define FLO_HTML_PARSER_TYPE_ELEMENT_ELEMENT_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ELEMENT_SUCCESS,
    ELEMENT_CREATED,
    ELEMENT_FOUND,
    ELEMENT_MEMORY,
    ELEMENT_NOT_FOUND_OR_CREATED,
    ELEMENT_TOO_LONG,
    ELEMENT_ARRAY_FULL,
    ELEMENT_NUM_STATUS
} flo_html_ElementStatus;

static const char *const elementStatusStrings[ELEMENT_NUM_STATUS] = {
    "Success",
    "Element created",
    "Element found",
    "Memory error detected",
    "Element not found or created",
    "Element too long",
    "Element array max size reached",
};

// Not always used, but very handy for those that actually do want readable
// error codes.
__attribute__((unused)) static const char *
flo_html_elementStatusToString(flo_html_ElementStatus status) {
    if (status >= 0 && status < ELEMENT_NUM_STATUS) {
        return elementStatusStrings[status];
    }
    return "Unknown element status code!";
}

#ifdef __cplusplus
}
#endif

#endif
