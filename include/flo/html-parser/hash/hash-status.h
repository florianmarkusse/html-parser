#ifndef FLO_HTML_PARSER_HASH_HASH_STATUS_H
#define FLO_HTML_PARSER_HASH_HASH_STATUS_H

typedef enum {
    HASH_SUCCESS,
    HASH_ERROR_MEMORY,
    HASH_MAX_PROBES,
    HASH_ERROR_CAPACITY,
    HASH_NUM_STATUS
} flo_html_HashStatus;

static const char *const hashStatusStrings[HASH_NUM_STATUS] = {
    "Success", "Memory Error", "Reached maximum number of probes",
    "Reached/Over capacity"};

__attribute__((unused)) static const char *
flo_html_hashStatusToString(flo_html_HashStatus status) {
    if (status >= 0 && status < HASH_NUM_STATUS) {
        return hashStatusStrings[status];
    }
    return "Unknown hash status code!";
}

#endif
