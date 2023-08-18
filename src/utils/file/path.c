#include <string.h>
#include <sys/stat.h>

#include "flo/html-parser/utils/file/file-status.h"
#include "flo/html-parser/utils/print/error.h"

#define FULL_ACCESS 0700

void createPath(const char *fileLocation) {
    const char *lastSlash = strrchr(fileLocation, '/');
    if (lastSlash != NULL) {
        // Calculate the length of the directory path
        size_t dirPathLength = lastSlash - fileLocation + 1;

        // Create a temporary buffer to store the directory path
        char dirPath[dirPathLength + 1];
        strncpy(dirPath, fileLocation, dirPathLength);
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
