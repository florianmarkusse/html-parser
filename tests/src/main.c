#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "comparisons/comparisons.h"

int main() {
    printf("Starting test suite...\n\n");

    size_t successes = 0;
    size_t failures = 0;
    testComparisons(&successes, &failures);

    if (failures > 0) {
        printf("\nTest suite failed.\n");
    } else {
        printf("\nTest suite successful.\n");
    }

    return failures > 0;
}
