#include <stdio.h>

#include "dom/document-user.h"
#include "dom/document-writing.h"
#include "dom/document.h"
#include "dom/querying/document-querying.h"
#include "querying/querying.h"
#include "test-status.h"
#include "test.h"
#include "type/element/elements.h"

#define CURRENT_DIR "tests/src/querying/inputs/"
#define TEST_FILE_1 CURRENT_DIR "test-1.html"

TestStatus testQuery(const char *fileLocation, const char *cssQuery,
                     const QueryingStatus expected) {
    DataContainer dataContainer;
    createDataContainer(&dataContainer);

    Document doc;
    if (createFromFile(fileLocation, &doc, &dataContainer) !=
        DOCUMENT_SUCCESS) {
        destroyDataContainer(&dataContainer);
        return TEST_ERROR_INITIALIZATION;
    }

    //    printGlobalTagStatus();
    printDocumentStatus(&doc, &dataContainer);
    printAttributeStatus(&dataContainer);
    //    printHTML(&doc);
    QueryingStatus actual = querySelectorAll(cssQuery, &doc, &dataContainer);
    const char *actualStatus = queryingStatusToString(actual);
    printf("recevied the following result code: %s\n", actualStatus);

    // TODO(florian): need to free the result from querySelectors..

    TestStatus result = TEST_FAILURE;

    if (actual == expected) {
        printTestSuccess();
        result = TEST_SUCCESS;
    } else {
        printTestFailure();
        printTestDemarcation();
        printTestResultDifference(expected, queryingStatusToString(expected),
                                  actual, queryingStatusToString(actual));
        printTestDemarcation();
    }

    destroyDocument(&doc);
    destroyDataContainer(&dataContainer);

    return result;
}

static inline void testAndCount(const char *fileLocation, const char *cssQuery,
                                const QueryingStatus expected,
                                const char *testName, size_t *localSuccsses,
                                size_t *localFailures) {
    printTestStart(testName);

    if (testQuery(fileLocation, cssQuery, expected) == TEST_SUCCESS) {
        (*localSuccsses)++;
    } else {
        (*localFailures)++;
    }
}

unsigned char testQueries(size_t *successes, size_t *failures) {
    printTestTopicStart("document queries");
    size_t localSuccesses = 0;
    size_t localFailures = 0;

    // testAndCount(TEST_FILE_1, "body div p h1 lalalal input",
    // QUERYING_SUCCESS,
    //              "tag selector", &localSuccesses, &localFailures);
    testAndCount(TEST_FILE_1, "body[required][text=free] div p      ",
                 QUERYING_SUCCESS, "tag selector", &localSuccesses,
                 &localFailures);

    printTestScore(localSuccesses, localFailures);

    *successes += localSuccesses;
    *failures += localFailures;

    return localFailures > 0;
}
