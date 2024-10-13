#include <string.h>
#include <sys/stat.h>

#include "flo/util/file/path.h"

#define FULL_ACCESS 0700

void flo_createPath(flo_String fileLocation, flo_Arena scratch) {
    ptrdiff_t currentIndex = 0;
    ptrdiff_t slashIndex =
        flo_firstOccurenceOfFrom(fileLocation, '/', currentIndex);
    if (slashIndex >= 0) {
        char *dirPath = FLO_NEW(&scratch, char, fileLocation.len + 1);
        memcpy(dirPath, fileLocation.buf, fileLocation.len);
        dirPath[fileLocation.len] = '\0';

        while (slashIndex > 0) {
            dirPath[slashIndex] =
                '\0'; // Temporarily terminate the string at the next slash
            mkdir(dirPath, FULL_ACCESS); // Create the directory
            dirPath[slashIndex] = '/';

            currentIndex = slashIndex + 1;
            slashIndex =
                flo_firstOccurenceOfFrom(fileLocation, '/', currentIndex);
        }
    }
}
