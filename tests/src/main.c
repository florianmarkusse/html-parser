#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "dom/appending/appending.h"
#include "dom/comparing/comparing.h"
#include "dom/deleting/deleting.h"
#include "dom/parsing/parsing.h"
#include "dom/prepending/prepending.h"
#include "dom/querying/querying.h"
#include "dom/replacing/replacing.h"
#include "integration-test.h"
#include "node/deleting/deleting.h"
#include "node/modifying/modifying.h"
#include "node/querying/querying.h"
#include "pretty-print.h"
#include "test.h"

int main() {
    printf("Starting test suite...\n\n");

    size_t successes = 0;
    size_t failures = 0;

    //    testflo_html_DomParsings(&successes, &failures);
    //    printf("\n");

    testflo_html_DomComparisons(&successes, &failures);
    printf("\n");
    //
    //    testflo_html_DomQueries(&successes, &failures);
    //    printf("\n");
    //
    //    testNodeQueries(&successes, &failures);
    //    printf("\n");
    //
    //    testflo_html_DomDeletions(&successes, &failures);
    //    printf("\n");
    //
    //    testNodeModifications(&successes, &failures);
    //    printf("\n");
    //
    //    testNodeDeletions(&successes, &failures);
    //    printf("\n");
    //
    //    testflo_html_DomAppendices(&successes, &failures);
    //    printf("\n");
    //
    //    testflo_html_DomPrependices(&successes, &failures);
    //    printf("\n");
    //
    //    testflo_html_DomReplacements(&successes, &failures);
    //    printf("\n");
    //
    //    testIntegrations(&successes, &failures);
    //    printf("\n");

    printTestScore(successes, failures);
    if (failures > 0) {
        printf("Test suite %sfailed%s.\n", ANSI_COLOR_RED, ANSI_COLOR_RESET);
    } else {
        printf("Test suite %ssuccessful%s.\n", ANSI_COLOR_GREEN,
               ANSI_COLOR_RESET);
    }

    return failures > 0;
}
