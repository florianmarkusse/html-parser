#include <dirent.h>
#include <flo/html-parser.h>
#include <flo/html-parser/utils/memory/memory.h>
#include <stdio.h>
#include <string.h>

#include "dom/parsing/parsing.h"
#include "test-status.h"
#include "test.h"

#define INPUTS_DIR "tests/src/dom/parsing/inputs/"
#define TEST_1 CURRENT_DIR "test-1.html"

unsigned char parseFile(const char *fileLocation) {
    TextStore textStore;
    ElementStatus initStatus = createTextStore(&textStore);
    if (initStatus != ELEMENT_SUCCESS) {
        ERROR_WITH_CODE_ONLY(elementStatusToString(initStatus),
                             "Failed to initialize text store");
        return TEST_ERROR_INITIALIZATION;
    }

    Dom dom1;
    if (createDomFromFile(fileLocation, &dom1, &textStore) != DOM_SUCCESS) {
        destroyTextStore(&textStore);
        return 0;
    }
    destroyDom(&dom1);
    destroyTextStore(&textStore);
    return 1;
}

static inline void testAndCount(size_t *localSuccesses, size_t *localFailures) {
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
        if (!parseFile(fileLocation)) {
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

unsigned char testDomParsings(size_t *successes, size_t *failures) {
    printTestTopicStart("DOM parsings");
    size_t localSuccesses = 0;
    size_t localFailures = 0;

    testAndCount(&localSuccesses, &localFailures);

    printTestScore(localSuccesses, localFailures);

    *successes += localSuccesses;
    *failures += localFailures;

    return localFailures > 0;
}
