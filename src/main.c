#include <stdio.h>
#include <stdlib.h>

#include "nodes.h"
#include "utils/file/read.h"

int main() {
    const char *htmlString = readFile("test/test.html");
    printf("%s\n", htmlString);
    parseNodes(htmlString);

    printNodes();
    cleanup();
}
