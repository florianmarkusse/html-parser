#include <string.h>
#include <sys/stat.h>

#include "flo/html-parser/util/file/path.h"

#define FULL_ACCESS 0700

void flo_html_createPath(const flo_html_String fileLocation) {
    // casting to char* because file location should not contain any funny
    // characters.
    char *buffer = (char *)fileLocation.buf;
    const char *lastSlash = strrchr(buffer, '/');
    if (lastSlash != NULL) {
        // Calculate the length of the directory path
        ptrdiff_t dirPathLength = lastSlash - buffer + 1;

        // Create a temporary buffer to store the directory path
        char dirPath[dirPathLength + 1];
        strncpy(dirPath, buffer, dirPathLength);
        dirPath[dirPathLength] = '\0';

        // Create parent directories if they don't exist
        char *currentDir = dirPath;
        char *nextSlash = NULL;
        while ((nextSlash = strchr(currentDir, '/')) != NULL) {
            *nextSlash =
                '\0'; // Temporarily terminate the string at the next slash
            mkdir(dirPath, FULL_ACCESS); // Create the directory
            *nextSlash = '/';            // Restore the next slash
            currentDir = nextSlash + 1;  // Move to the next directory
        }

        // Create the final directory
        mkdir(dirPath, FULL_ACCESS);
    }
}
