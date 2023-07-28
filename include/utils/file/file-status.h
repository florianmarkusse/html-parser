#ifndef UTILS_FILE_FILE_STATUS_H
#define UTILS_FILE_FILE_STATUS_H

typedef enum {
    FILE_SUCCESS,
    FILE_CANT_OPEN,
    FILE_CANT_ALLOCATE,
    FILE_CANT_READ,
    FILE_CANT_CREATE,
    FILE_CANT_WRITE,
    FILE_NUM_STATUS
} FileStatus;

static const char *const FileStatusStrings[FILE_NUM_STATUS] = {
    "Success",          "Cannot open file",   "Cannot allocate memory",
    "Cannot read file", "Cannot create file", "Cannot write to file"};

// Not always used, but very handy for those that actually do want readable
// error codes.
__attribute__((unused)) static const char *
fileStatusToString(FileStatus status) {
    if (status >= 0 && status < FILE_NUM_STATUS) {
        return FileStatusStrings[status];
    }
    return "Unknown file status code!";
}

#endif
