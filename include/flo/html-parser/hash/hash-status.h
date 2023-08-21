#ifndef FLO_HTML_PARSER_HASH_HASH_STATUS_H
#define FLO_HTML_PARSER_HASH_HASH_STATUS_H

typedef enum {
    HASH_SUCCESS,
    HASH_ERROR_MEMORY,
    HASH_FULL_CAPACITY,
    HASH_NUM_STATUS
} HashStatus;

static const char *const HashStatusStrings[HASH_NUM_STATUS] = {
    "Success", "Memory Error", "Reached capacity"};

__attribute__((unused)) static const char *
hashStatusToString(HashStatus status) {
    if (status >= 0 && status < HASH_NUM_STATUS) {
        return HashStatusStrings[status];
    }
    return "Unknown hash status code!";
}

#endif