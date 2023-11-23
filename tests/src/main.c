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
        FLO_PRINT_ERROR("Failed to allocate memory!\n");
        return -1;
    }

    flo_Arena arena = flo_createArena(begin, CAP);

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
        return -1;
    }
    arena.jmp_buf = jmp_buf;

    ptrdiff_t successes = 0;
    ptrdiff_t failures = 0;

    for (ptrdiff_t i = 0; i < 1; i++) {
        // FLO_LOG(FLO_STRING("hello word"), false, FLO_STDOUT);
        // FLO_LOG_1(FLO_STRING("hello word"));
        //        FLO_LOG(toPrint);
        //        FLO_LOG((FLO_STRING("test")), 0);
        ptrdiff_t myValue = 10043340089044;
        FLO_LOG(myValue);
        // FLO_LOG(FLO_STRING("hello word"), true, FLO_STDERR);
        //        FLO_LOG(FLO_STRING("hello word"), true);
        //        FLO_LOG(FLO_STDOUT, FLO_STRING("hello word"), true);
    }

    FLO_FLUSH_TO(FLO_STDOUT);

    //    writestuff(FLO_STDOUT, FLO_STRING("hello word"));
    //    writestuff(FLO_STDERR, FLO_STRING("hello word"));

    //    testflo_html_DomParsings(&successes, &failures, arena);
    //    printf("\n");
    //
    //    testflo_html_DomComparisons(&successes, &failures, arena);
    //    printf("\n");
    //
    //    testflo_html_DomQueries(&successes, &failures, arena);
    //    printf("\n");
    //
    //    testNodeQueries(&successes, &failures, arena);
    //    printf("\n");
    //
    //    testflo_html_DomDeletions(&successes, &failures, arena);
    //    printf("\n");
    //
    //    testNodeModifications(&successes, &failures, arena);
    //    printf("\n");
    //
    //    testNodeDeletions(&successes, &failures, arena);
    //    printf("\n");
    //
    //    testflo_html_DomAppendices(&successes, &failures, arena);
    //    printf("\n");
    //
    //    testflo_html_DomPrependices(&successes, &failures, arena);
    //    printf("\n");
    //
    //    testflo_html_DomReplacements(&successes, &failures, arena);
    //    printf("\n");
    //
    //    testflo_html_DomDuplications(&successes, &failures, arena);
    //    printf("\n");
    //
    //    testIntegrations(&successes, &failures, arena);
    //    printf("\n");
    //
    //    printTestScore(successes, failures);
    //    if (failures > 0) {
    //        printf("Test suite %sfailed%s.\n", ANSI_COLOR_RED,
    //        ANSI_COLOR_RESET);
    //    } else {
    //        printf("Test suite %ssuccessful%s.\n", ANSI_COLOR_GREEN,
    //               ANSI_COLOR_RESET);
    //    }

    return failures > 0;
}
