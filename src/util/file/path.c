#include <string.h>
#include <sys/stat.h>

#include "flo/html-parser/util/file/path.h"

#define FULL_ACCESS 0700

void flo_html_createPath(const flo_html_String fileLocation) {
    ptrdiff_t currentIndex = 0;
    ptrdiff_t slashIndex =
        flo_html_firstOccurenceOfFrom(fileLocation, '/', currentIndex);
    if (slashIndex >= 0) {
        char dirPath[fileLocation.len + 1];
        memcpy(dirPath, fileLocation.buf, fileLocation.len);
        dirPath[fileLocation.len] = '\0';

        while (slashIndex > 0) {
            dirPath[slashIndex] =
                '\0'; // Temporarily terminate the string at the next slash
            mkdir(dirPath, FULL_ACCESS); // Create the directory
            dirPath[slashIndex] = '/';

            currentIndex = slashIndex + 1;
            slashIndex =
                flo_html_firstOccurenceOfFrom(fileLocation, '/', currentIndex);
        }
    }
}
