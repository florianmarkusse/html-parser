#include <stdio.h>
#include <stdlib.h>

#include "utils/file/read.h"

int main() {
    size_t dataLen = 0;
    const char *data = readFile("test/test.html", &dataLen);

    printf("%s\n", data);

    free((void *)data);
}
