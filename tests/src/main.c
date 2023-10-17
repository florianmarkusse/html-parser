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

    flo_html_Arena arena = flo_html_newArena(1U << 27U);
    void *jmp_buf[5];
    if (__builtin_setjmp(jmp_buf)) {
        flo_html_destroyArena(&arena);
        FLO_HTML_PRINT_ERROR("OOM in arena!\n");
        return -1;
    }
    arena.jmp_buf = jmp_buf;

    ptrdiff_t successes = 0;
    ptrdiff_t failures = 0;

    testflo_html_DomParsings(&successes, &failures, arena);
    printf("\n");

    testflo_html_DomComparisons(&successes, &failures, arena);
    printf("\n");

    testflo_html_DomQueries(&successes, &failures, arena);
    printf("\n");

    testNodeQueries(&successes, &failures, arena);
    printf("\n");

    testflo_html_DomDeletions(&successes, &failures, arena);
    printf("\n");

    testNodeModifications(&successes, &failures, arena);
    printf("\n");

    testNodeDeletions(&successes, &failures, arena);
    printf("\n");

    testflo_html_DomAppendices(&successes, &failures, arena);
    printf("\n");

    testflo_html_DomPrependices(&successes, &failures, arena);
    printf("\n");

    testflo_html_DomReplacements(&successes, &failures, arena);
    printf("\n");

    testIntegrations(&successes, &failures, arena);
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
