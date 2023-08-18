

#include <dirent.h>
#include <dom/document-user.h>
#include <dom/document-writing.h>
#include <dom/document.h>
#include <stdio.h>
#include <string.h>

#include "parsing/parsing.h"
#include "test-status.h"
#include "test.h"

#define INPUTS_DIR "tests/src/parsing/inputs/"
#define TEST_1 CURRENT_DIR "test-1.html"

unsigned char parseFile(const char *fileLocation) {
    DataContainer dataContainer;
    createDataContainer(&dataContainer);

    Document doc1;
    if (createFromFile(fileLocation, &doc1, &dataContainer) !=
        DOCUMENT_SUCCESS) {
        destroyDataContainer(&dataContainer);
        return 0;
    }
    destroyDocument(&doc1);
    destroyDataContainer(&dataContainer);
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
            printf("Parsing document %s failed\n", fileLocation);
            printTestDemarcation();
        } else {
            (*localSuccesses)++;
            printTestSuccess();
        }
    }

    closedir(dir);
}

unsigned char testParsings(size_t *successes, size_t *failures) {
    printTestTopicStart("document parsings");
    size_t localSuccesses = 0;
    size_t localFailures = 0;

    testAndCount(&localSuccesses, &localFailures);

    printTestScore(localSuccesses, localFailures);

    *successes += localSuccesses;
    *failures += localFailures;

    return localFailures > 0;
}
