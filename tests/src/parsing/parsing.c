

#include <stdio.h>

#include "dom/document-user.h"
#include "dom/document.h"
#include "parsing/parsing.h"
#include "test-status.h"
#include "test.h"

#define CURRENT_DIR "tests/src/parsing/"
#define LARGE_FILE CURRENT_DIR "large-file.html"

TestStatus parseFiles(const char *fileLocation1) {
    createGlobals();

    Document doc1;
    if (createFromFile(fileLocation1, &doc1) != DOCUMENT_SUCCESS) {
        destroyGlobals();
        return TEST_FAILURE;
    }

    destroyDocument(&doc1);
    destroyGlobals();

    return TEST_SUCCESS;
}

static inline void testAndCount(const char *fileLocation1,

                                const char *testName, size_t *localSuccesses,
                                size_t *localFailures) {
    printTestStart(testName);

    if (parseFiles(fileLocation1) == TEST_SUCCESS) {
        (*localSuccesses)++;
    } else {
        (*localFailures)++;
    }
}

unsigned char testParsings(size_t *successes, size_t *failures) {
    printTestTopicStart("document parsings");
    size_t localSuccesses = 0;
    size_t localFailures = 0;

    testAndCount(LARGE_FILE, "large file", &localSuccesses, &localFailures);

    printTestScore(localSuccesses, localFailures);

    *successes += localSuccesses;
    *failures += localFailures;

    return localFailures > 0;
}
