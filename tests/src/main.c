#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>

#include "dom/appending/appending.h"
#include "dom/comparing/comparing.h"
#include "dom/deleting/deleting.h"
#include "dom/duplicating/duplicating.h"
#include "dom/parsing/parsing.h"
#include "dom/prepending/prepending.h"
#include "dom/querying/querying.h"
#include "dom/replacing/replacing.h"
#include "error.h"
#include "hash/hashes.h"
#include "hash/msi/string-set.h"
#include "hash/trie/string-set.h"
#include "hash/trie/uint16-set.h"
#include "integration-test.h"
#include "log.h"
#include "node/deleting/deleting.h"
#include "node/modifying/modifying.h"
#include "node/querying/querying.h"
#include "pretty-print.h"
#include "test.h"

#define CAP 1 << 21

int main() {
    printf("Starting test suite...\n\n");

    char *begin = mmap(NULL, CAP, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (begin == MAP_FAILED) {
        FLO_ERROR("Failed to allocate memory!\n", FLO_FLUSH);
        return -1;
    }

    flo_Arena arena = flo_createArena(begin, CAP);

    void *jmp_buf[5];
    if (__builtin_setjmp(jmp_buf)) {
        if (munmap(arena.beg, arena.cap) == -1) {
            FLO_FLUSH_AFTER(FLO_STDERR) {
                FLO_ERROR((FLO_STRING("Failed to unmap memory from arena!\n"
                                      "Arena Details:\n"
                                      "  beg: ")));
                FLO_ERROR(arena.beg);
                FLO_ERROR((FLO_STRING("\n end: ")));
                FLO_ERROR(arena.end);
                FLO_ERROR((FLO_STRING("\n cap: ")));
                FLO_ERROR(arena.cap);
                FLO_ERROR((FLO_STRING("\nZeroing Arena regardless.\n")));
            }
        }
        arena.beg = NULL;
        arena.end = NULL;
        arena.cap = 0;
        arena.jmp_buf = NULL;
        FLO_ERROR((FLO_STRING("OOM/overflow in arena!\n")), FLO_FLUSH);
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

    testflo_html_DomDuplications(&successes, &failures, arena);
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
