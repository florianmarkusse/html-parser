#ifndef FLO_HTML_PARSER_USER_STATUS_H
#define FLO_HTML_PARSER_USER_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    USER_SUCCESS,
    USER_MEMORY,
    USER_FILE_FAIL,
    USER_NUM_STATUS
} flo_html_UserStatus;

static const char *const userStatusStrings[USER_NUM_STATUS] = {
    "Success", "Memory error", "Creating DOM from file failed"};

__attribute__((unused)) static const char *
flo_html_userStatusToString(flo_html_UserStatus status) {
    if (status >= 0 && status < USER_NUM_STATUS) {
        return userStatusStrings[status];
    }
    return "Unknown user status code!";
}

#ifdef __cplusplus
}
#endif

#endif
