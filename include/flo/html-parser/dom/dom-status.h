#ifndef FLO_HTML_PARSER_DOM_DOM_STATUS_H
#define FLO_HTML_PARSER_DOM_DOM_STATUS_H

typedef enum {
    DOM_SUCCESS,
    DOM_ERROR_MEMORY,
    DOM_NO_ADD,
    DOM_NO_ELEMENT,
    DOM_TOO_DEEP,
    DOM_TOO_MANY_ATTRIBUTES,
    DOM_NUM_STATUS
} flo_html_DomStatus;

static const char *const domStatusStrings[DOM_NUM_STATUS] = {
    "Success",
    "Memory",
    "Could not add to dom",
    "Could not find element",
    "Too many nested tags",
    "Too many attributes on single tag"};

// Not always used, but very handy for those that actually do want readable
// error codes.
__attribute__((unused)) static const char *
documentStatusToString(flo_html_DomStatus status) {
    if (status >= 0 && status < DOM_NUM_STATUS) {
        return domStatusStrings[status];
    }
    return "Unknown dom status code!";
}

#endif
