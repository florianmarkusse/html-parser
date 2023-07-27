#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "comparisons/comparisons.h"
#include "pretty-print.h"

int main() {
    printf("Starting test suite...\n\n");

    size_t successes = 0;
    size_t failures = 0;

    testComparisons(&successes, &failures);
    printf("\n");

    printf("[ %zu / %lu ]\n", successes, failures + successes);
    if (failures > 0) {
        printf("Test suite %sfailed%s.\n", ANSI_COLOR_RED, ANSI_COLOR_RESET);
    } else {
        printf("Test suite %ssuccessful%s.\n", ANSI_COLOR_GREEN,
               ANSI_COLOR_RESET);
    }

    return failures > 0;
}
