#ifndef FLO_UTIL_FILE_FILE_STATUS_H
#define FLO_UTIL_FILE_FILE_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FILE_SUCCESS,
    FILE_CANT_OPEN,
    FILE_CANT_ALLOCATE,
    FILE_CANT_READ,
    FILE_NUM_STATUS
} flo_FileStatus;

static char *fileStatusStrings[FILE_NUM_STATUS] = {
    "Success",
    "Cannot open file",
    "Cannot allocate memory",
    "Cannot read file",
};

// Not always used, but very handy for those that actually do want readable
// error codes.
__attribute__((unused)) static char *
flo_fileStatusToString(flo_FileStatus status) {
    if (status >= 0 && status < FILE_NUM_STATUS) {
        return fileStatusStrings[status];
    }
    return "Unknown file status code!";
}

#ifdef __cplusplus
}
#endif

#endif
