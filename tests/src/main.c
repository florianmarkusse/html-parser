#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

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

#define CAP 1 << 27

flo_Arena setupArena() {
    char *start = mmap(NULL, CAP, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (start == MAP_FAILED) {
        FLO_PRINT_ERROR("Failed to allocate memory!\n");
        return (flo_Arena){0};
    }

    flo_Arena arena = flo_createArena(start, CAP);

    void *jmp_buf[5];
    if (__builtin_setjmp(jmp_buf)) {
        if (munmap(arena.beg, arena.cap) == -1) {
            FLO_PRINT_ERROR("Failed to unmap memory from arena!\n"
                            "Arena Details:\n"
                            "  beg: %p\n"
                            "  end: %p\n"
                            "  cap: %td\n"
                            "Zeroing Arena regardless.",
                            arena.beg, arena.end, arena.cap);
        }
        FLO_PRINT_ERROR("OOM/overflow in arena!\n");
        return (flo_Arena){0};
    }
    arena.jmp_buf = jmp_buf;

    return arena;
}

int main() {
    printf("Starting test suite...\n\n");

    flo_Arena arena = setupArena();
    if (arena.beg == NULL) {
        return 1;
    }

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
