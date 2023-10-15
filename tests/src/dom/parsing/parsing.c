#include <dirent.h>
#include <flo/html-parser.h>
#include <flo/html-parser/util/memory.h>
#include <stdio.h>
#include <string.h>

#include "dom/parsing/parsing.h"
#include "test-status.h"
#include "test.h"

#define INPUTS_DIR "tests/src/dom/parsing/inputs/"
#define TEST_1 CURRENT_DIR "test-1.html"

bool parseFile(const flo_html_String fileLocation, flo_html_Arena scratch) {
    flo_html_ParsedHTML parsed;
    if (flo_html_fromFile(fileLocation, &parsed, &scratch) != USER_SUCCESS) {
        return false;
    }

    return true;
}

static inline void testAndCount(ptrdiff_t *localSuccesses,
                                ptrdiff_t *localFailures,
                                flo_html_Arena scratch) {
    DIR *dir = NULL;
    struct dirent *ent = NULL;
    if ((dir = opendir(INPUTS_DIR)) == NULL) {
        (*localFailures)++;
        printf("Failed to open test directory: %s\n", INPUTS_DIR);
        return;
    }

    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;
        }
        char fileLocation[1024];
        snprintf(fileLocation, sizeof(fileLocation), "%s%s", INPUTS_DIR,
                 ent->d_name);
        printTestStart(fileLocation);
        if (!parseFile(FLO_HTML_S_LEN(fileLocation, strlen(fileLocation)),
                       scratch)) {
            (*localFailures)++;
            printTestFailure();
            printTestDemarcation();
            printf("Parsing DOM %s failed\n", fileLocation);
            printTestDemarcation();
        } else {
            (*localSuccesses)++;
            printTestSuccess();
        }
    }

    closedir(dir);
}

bool testflo_html_DomParsings(ptrdiff_t *successes, ptrdiff_t *failures,
                              flo_html_Arena scratch) {
    printTestTopicStart("DOM parsings");
    ptrdiff_t localSuccesses = 0;
    ptrdiff_t localFailures = 0;

    testAndCount(&localSuccesses, &localFailures, scratch);

    printTestScore(localSuccesses, localFailures);

    *successes += localSuccesses;
    *failures += localFailures;

    return localFailures > 0;
}
