#ifndef UTILS_FILE_FILE_STATUS_H
#define UTILS_FILE_FILE_STATUS_H

typedef enum {
    FILE_SUCCESS,
    FILE_CANT_OPEN,
    FILE_CANT_ALLOCATE,
    FILE_CANT_READ,
    FILE_NUM_STATUS
} FileStatus;

static const char *const FileStatusStrings[FILE_NUM_STATUS] = {
    "Success",
    "Cannot open file",
    "Cannot allocate memory",
    "Cannot read file",
};

static const char *fileStatusToString(FileStatus status) {
    if (status >= 0 && status < FILE_NUM_STATUS) {
        return FileStatusStrings[status];
    }
    return "Unknown file status code!";
}

#endif
