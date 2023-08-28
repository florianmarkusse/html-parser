#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "dom/comparing/comparing.h"
#include "dom/parsing/parsing.h"
#include "dom/querying/querying.h"
#include "node/querying/querying.h"
#include "pretty-print.h"
#include "test.h"

int main() {
    printf("Starting test suite...\n\n");

    size_t successes = 0;
    size_t failures = 0;

    testDomParsings(&successes, &failures);
    printf("\n");

    testDomComparisons(&successes, &failures);
    printf("\n");

    testDomQueries(&successes, &failures);
    printf("\n");

    testNodeQueries(&successes, &failures);
    printf("\n");

    printTestScore(successes, failures);
    if (failures > 0) {
        printf("Test suite %sfailed%s.\n", ANSI_COLOR_RED, ANSI_COLOR_RESET);
    } else {
        printf("Test suite %ssuccessful%s.\n", ANSI_COLOR_GREEN,
               ANSI_COLOR_RESET);
    }

    return failures > 0;
}
