#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>

#include "dom/appending/appending.h"
#include "dom/comparing/comparing.h"
#include "dom/deleting/deleting.h"
#include "dom/parsing/parsing.h"
#include "dom/prepending/prepending.h"
#include "dom/querying/querying.h"
#include "dom/replacing/replacing.h"
#include "error.h"
#include "hash/hashes.h"
#include "hash/msi/string-hash.h"
#include "hash/string-hash.h"
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

void rehashIndex(flo_msi_String *oldIndex, flo_msi_String *newIndex) {
    FLO_ASSERT(newIndex->len == 0);
    for (int32_t i = 0; i < (1 << oldIndex->exp); i++) {
        flo_String s = oldIndex->buf[i];
        if (s.len > 0) {
            flo_msi_insertString(s, flo_hashString(s), newIndex);
        }
    }
}

bool indexInsert(flo_String string, flo_msi_String *index, flo_Arena *perm) {
    if ((uint32_t)index->len >= ((uint32_t)1 << index->exp) / 2) {
        flo_msi_String newIndex = (flo_msi_String){.exp = index->exp + 1};
        flo_msi_newSet(&newIndex, FLO_SIZEOF(*newIndex.buf),
                       FLO_ALIGNOF(*newIndex.buf), perm);
        rehashIndex(index, &newIndex);
        *index = newIndex;
    }
    return flo_msi_insertString(string, flo_hashString(string), index);
}

char *generateRandomString(int length, int index) {
    static const char charset[] =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    char *randomString = malloc((length + 1) * sizeof(char));

    if (randomString == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    srand((unsigned int)time(NULL) + index); // Seed the random number generator

    for (int i = 0; i < length; ++i) {
        int index = rand() % (int)(sizeof(charset) - 1);
        randomString[i] = charset[index];
    }

    randomString[length] = '\0'; // Null-terminate the string

    return randomString;
}

int main() {
    printf("Starting test suite...\n\n");

    flo_Arena arena = setupArena();
    if (arena.beg == NULL) {
        return 1;
    }

    flo_msi_String index = FLO_NEW_MSI_SET(flo_msi_String, 1, &arena);

    for (int i = 0; i < 20; ++i) {
        char *randomString = generateRandomString(10, i % 5);
        indexInsert(FLO_STRING_LEN(randomString, strlen(randomString)), &index,
                    &arena);
    }

    FLO_PRINT_ERROR("Size of set is now %td\n", index.len);

    flo_String element;
    FOR_EACH_MSI_ELEMENT(element, index) {
        FLO_PRINT_ERROR("string with vlaue is %.*s\n",
                        FLO_STRING_PRINT(element));
    }

    //    ptrdiff_t successes = 0;
    //    ptrdiff_t failures = 0;
    //
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
    //
    //    return failures > 0;
}
