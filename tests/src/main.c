#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "comparisons/comparisons.h"
#include "parsing/parsing.h"
#include "pretty-print.h"
#include "querying/querying.h"
#include "test.h"

int main() {
    printf("Starting test suite...\n\n");

    size_t successes = 0;
    size_t failures = 0;

    //    testParsings(&successes, &failures);
    //    printf("\n");

    //    testComparisons(&successes, &failures);
    //    printf("\n");

    testQueries(&successes, &failures);
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
