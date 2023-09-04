
#include <flo/html-parser/dom/appendix/appendix.h>
#include <flo/html-parser/dom/comparison/comparison.h>
#include <flo/html-parser/dom/deletion/deletion.h>
#include <flo/html-parser/dom/modification/modification.h>
#include <flo/html-parser/dom/query/query.h>
#include <flo/html-parser/dom/user.h>
#include <flo/html-parser/dom/writing.h>
#include <flo/html-parser/utils/print/error.h>
#include <stdbool.h>
#include <stdio.h>

#include "comparison-test.h"
#include "dom/appending/appending.h"
#include "test-status.h"
#include "test.h"

#define CURRENT_DIR "tests/src/dom/appending/inputs/"
#define TEST_FILE_1_BEFORE CURRENT_DIR "test-1-before.html"
#define TEST_FILE_1_AFTER CURRENT_DIR "test-1-after.html"
#define TEST_FILE_2_BEFORE CURRENT_DIR "test-2-before.html"
#define TEST_FILE_2_AFTER CURRENT_DIR "test-2-after.html"
#define TEST_FILE_3_BEFORE CURRENT_DIR "test-3-before.html"
#define TEST_FILE_3_AFTER CURRENT_DIR "test-3-after.html"
#define TEST_FILE_4_BEFORE CURRENT_DIR "test-4-before.html"
#define TEST_FILE_4_AFTER CURRENT_DIR "test-4-after.html"
#define TEST_FILE_5_BEFORE CURRENT_DIR "test-5-before.html"
#define TEST_FILE_5_AFTER CURRENT_DIR "test-5-after.html"
#define TEST_FILE_6_BEFORE CURRENT_DIR "test-6-before.html"
#define TEST_FILE_6_AFTER CURRENT_DIR "test-6-after.html"
#define TEST_FILE_7_BEFORE CURRENT_DIR "test-7-before.html"
#define TEST_FILE_7_AFTER CURRENT_DIR "test-7-after.html"
#define TEST_FILE_8_BEFORE CURRENT_DIR "test-8-before.html"
#define TEST_FILE_8_AFTER CURRENT_DIR "test-8-after.html"
#define TEST_FILE_9_BEFORE CURRENT_DIR "test-9-before.html"
#define TEST_FILE_9_AFTER CURRENT_DIR "test-9-after.html"

typedef enum {
    APPEND_DOCUMENT_NODE,
    APPEND_TEXT_NODE,
    APPEND_FROM_STRING,
    NUM_APPEND_NODES
} AppendType;

typedef union {
    const AppendDocumentNode appendDocumentNode;
    const char *text;
} AppendInput;

typedef struct {
    const char *fileLocation1;
    const char *fileLocation2;
    const char *cssQuery;
    const char *testName;
    const AppendType appendType;
    const AppendInput appendInput;
} __attribute__((aligned(128))) TestFile;

static const TestFile testFiles[] = {
    {TEST_FILE_1_BEFORE,
     TEST_FILE_1_AFTER,
     "body",
     "document node to element with multiple children",
     APPEND_DOCUMENT_NODE,
     {{"example-tag",
       true,
       {"prop1", "prop2"},
       2,
       {"key1", "key2"},
       {"value1", "value2"},
       2}}},
    {TEST_FILE_2_BEFORE,
     TEST_FILE_2_AFTER,
     "div[special-one]",
     "document node to element with 1 child",
     APPEND_DOCUMENT_NODE,
     {{"example-tag", false, {}, 0, {"bla-bla"}, {"bla-bla"}, 1}}},
    {TEST_FILE_3_BEFORE,
     TEST_FILE_3_AFTER,
     "x",
     "document node to element with no children",
     APPEND_DOCUMENT_NODE,
     {{"example-tag", true, {"required", "help-me"}, 2, {}, {}, 0}}},
    {TEST_FILE_4_BEFORE,
     TEST_FILE_4_AFTER,
     "body",
     "text node to element with multiple children",
     APPEND_TEXT_NODE,
     {{"zoinks"}}},
    {TEST_FILE_5_BEFORE,
     TEST_FILE_5_AFTER,
     "div[special-one]",
     "text node to element with 1 child",
     APPEND_TEXT_NODE,
     {{"mama ce mama ca"}}},
    {TEST_FILE_6_BEFORE,
     TEST_FILE_6_AFTER,
     "x",
     "text node to element with no children",
     APPEND_TEXT_NODE,
     {{"my special text plan"}}},
    {TEST_FILE_7_BEFORE,
     TEST_FILE_7_AFTER,
     "body",
     "string to element with multiple children",
     APPEND_FROM_STRING,
     {{"<body style=\"newstyle\">"
       "  <div id=\"my-first-div\">"
       "    <p class=\"big\">Test text</p>"
       "  </div>"
       "  <section a b c d e f g h i>"
       "    <div special-one>"
       "      <span required></span>"
       "    </div>"
       "  </section>"
       "  <div class=\"big\" special-one></div>"
       "  <div class=\"big\"><p></p></div>"
       "  <div id=\"test\"></div>"
       "  <div class=\"big\"></div>"
       "  <section id=\"text-content-test\">"
       "    Hi ther"
       "    <div>"
       "      <span> Span this </span>"
       "      middle is mid"
       "    </div>"
       "    TEST"
       "    <p>my p big</p>"
       "  </section>"
       "  bazinga"
       "  <x></x>"
       "  <div></div>"
       "  <p html></p>"
       "  <input required text=\"free\" />"
       "</body>"}}},
    {TEST_FILE_8_BEFORE,
     TEST_FILE_8_AFTER,
     "div[special-one]",
     "string to element with 1 child",
     APPEND_FROM_STRING,
     {{"<whoop></whoop>"}}},
    {TEST_FILE_9_BEFORE,
     TEST_FILE_9_AFTER,
     "x",
     "string to element with no children",
     APPEND_FROM_STRING,
     {{"text only gang"}}},
};
static const size_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus testAppendix(const char *fileLocation1,
                               const char *fileLocation2, const char *cssQuery,
                               const AppendType appendType,
                               const AppendInput *appendInput) {
    TestStatus result = TEST_FAILURE;

    ComparisonTest comparisonTest;
    result = initComparisonTest(&comparisonTest, fileLocation1, fileLocation2);
    if (result != TEST_SUCCESS) {
        return result;
    }

    node_id foundNode = 0;
    result = getNodeFromQuerySelector(cssQuery, &comparisonTest, &foundNode);
    if (result != TEST_SUCCESS) {
        return result;
    }

    DomStatus domStatus = DOM_SUCCESS;
    switch (appendType) {
    case APPEND_DOCUMENT_NODE: {
        domStatus = appendDocumentNode(
            foundNode, &appendInput->appendDocumentNode,
            &comparisonTest.startDom, &comparisonTest.startDataContainer);
        break;
    }
    case APPEND_TEXT_NODE: {
        domStatus = appendTextNode(foundNode, appendInput->text,
                                   &comparisonTest.startDom,
                                   &comparisonTest.startDataContainer);
        break;
    }
    case APPEND_FROM_STRING: {
        domStatus = appendNodesFromString(foundNode, appendInput->text,
                                          &comparisonTest.startDom,
                                          &comparisonTest.startDataContainer);
        break;
    }
    default: {
        return failWithMessage("No suitable appendix type was supplied!\n",
                               &comparisonTest);
    }
    }

    if (domStatus != DOM_SUCCESS) {
        return failWithMessage("Failed to append document to node!\n",
                               &comparisonTest);
    }

    return compareAndEndTest(&comparisonTest);
}

bool testDomAppendices(size_t *successes, size_t *failures) {
    printTestTopicStart("DOM appendices");

    size_t localSuccesses = 0;
    size_t localFailures = 0;

    for (size_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];
        printTestStart(testFile.testName);

        if (testAppendix(testFile.fileLocation1, testFile.fileLocation2,
                         testFile.cssQuery, testFile.appendType,
                         &testFile.appendInput) != TEST_SUCCESS) {
            localFailures++;
        } else {
            localSuccesses++;
        }
    }

    printTestScore(localSuccesses, localFailures);

    *successes += localSuccesses;
    *failures += localFailures;

    return localFailures > 0;
}
