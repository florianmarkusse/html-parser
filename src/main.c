#include <stdio.h>
#include <stdlib.h>

#include "dom/document.h"
#include "utils/file/read.h"

int main() {
    const char *htmlString = readFile("test/test.html");
    printf("%s\n", htmlString);
    const Document doc = createDocument(htmlString);

    printDocument(&doc);

    destroyDocument(&doc);
}
