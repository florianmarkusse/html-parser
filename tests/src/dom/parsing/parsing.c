#include <dirent.h>
#include <flo/html-parser.h>
#include <memory/arena.h>
#include <stdio.h>
#include <string.h>

#include "dom/parsing/parsing.h"
#include "test.h"

#define INPUTS_DIR "tests/src/dom/parsing/inputs/"
#define FUZZ_INPUTS_DIR "tests/src/dom/parsing/fuzz-inputs/"
#define TEST_1 CURRENT_DIR "test-1.html"

bool parseFile(char *fileLocation, flo_Arena scratch) {
    flo_html_Dom *dom = flo_html_createDomFromFile(fileLocation, &scratch);
    if (dom == NULL) {
        return false;
    }

    return true;
}

static inline void testAndCount(char *directory, flo_Arena scratch) {
    DIR *dir = NULL;
    struct dirent *ent = NULL;
    if ((dir = opendir(directory)) == NULL) {
        FLO_TEST_FAILURE {
            FLO_ERROR((FLO_STRING("Failed to open test directory: ")));
            FLO_ERROR(directory, FLO_NEWLINE);
        }
        return;
    }

    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;
        }

        char fileLocation[1024];
        snprintf(fileLocation, sizeof(fileLocation), "%s%s", directory,
                 ent->d_name);
        FLO_TEST(FLO_STRING_LEN(fileLocation, strlen(fileLocation))) {
            if (!parseFile(fileLocation, scratch)) {
                FLO_TEST_FAILURE {
                    FLO_ERROR((FLO_STRING("Parsing DOM of file ")));
                    FLO_ERROR(fileLocation);
                    FLO_ERROR((FLO_STRING(" failed\n")));
                }
            } else {
                flo_testSuccess();
            }
        }
    }

    closedir(dir);
}

void testflo_html_DomParsings(flo_Arena scratch) {
    FLO_TEST_TOPIC(FLO_STRING("DOM parsings")) {
        // testAndCount(FUZZ_INPUTS_DIR, scratch);
        testAndCount(INPUTS_DIR, scratch);
    }
}
