
#include <flo/html-parser/dom/comparison/comparison.h>
#include <flo/html-parser/dom/deletion/deletion.h>
#include <flo/html-parser/dom/modification/modification.h>
#include <flo/html-parser/dom/query/query.h>
#include <flo/html-parser/dom/replacement/replacement.h>
#include <flo/html-parser/dom/user.h>
#include <flo/html-parser/dom/writing.h>
#include <flo/html-parser/utils/print/error.h>
#include <stdbool.h>
#include <stdio.h>

#include "comparison-test.h"
#include "dom/replacing/replacing.h"
#include "test-status.h"
#include "test.h"

#define CURRENT_DIR "tests/src/dom/replacing/inputs/"
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
#define TEST_FILE_10_BEFORE CURRENT_DIR "test-10-before.html"
#define TEST_FILE_10_AFTER CURRENT_DIR "test-10-after.html"
#define TEST_FILE_11_BEFORE CURRENT_DIR "test-11-before.html"
#define TEST_FILE_11_AFTER CURRENT_DIR "test-11-after.html"
#define TEST_FILE_12_BEFORE CURRENT_DIR "test-12-before.html"
#define TEST_FILE_12_AFTER CURRENT_DIR "test-12-after.html"

typedef enum {
    REPLACEMENT_DOCUMENT_NODE,
    REPLACEMENT_TEXT_NODE,
    REPLACEMENT_FROM_STRING,
    NUM_REPLACEMENT_TYPES
} ReplacementType;

typedef union {
    const DocumentNode documentNode;
    const char *text;
} ReplacementInput;

typedef struct {
    const char *fileLocation1;
    const char *fileLocation2;
    const char *cssQuery;
    const char *testName;
    const ReplacementType replacementType;
    const ReplacementInput replacementInput;
} __attribute__((aligned(128))) TestFile;

static const TestFile testFiles[] = {
    {TEST_FILE_1_BEFORE,
     TEST_FILE_1_AFTER,
     "body",
     "document node with element with multiple children",
     REPLACEMENT_DOCUMENT_NODE,
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
     "document node with element with 1 child",
     REPLACEMENT_DOCUMENT_NODE,
     {{"example-tag", false, {}, 0, {"bla-bla"}, {"bla-bla"}, 1}}},
    {TEST_FILE_3_BEFORE,
     TEST_FILE_3_AFTER,
     "x",
     "document node with element with no children",
     REPLACEMENT_DOCUMENT_NODE,
     {{"example-tag", true, {"required", "help-me"}, 2, {}, {}, 0}}},
    {TEST_FILE_4_BEFORE,
     TEST_FILE_4_AFTER,
     "body",
     "text node with element with multiple children",
     REPLACEMENT_TEXT_NODE,
     {{"zoinks"}}},
    {TEST_FILE_5_BEFORE,
     TEST_FILE_5_AFTER,
     "div[special-one]",
     "text node with element with 1 child",
     REPLACEMENT_TEXT_NODE,
     {{"mama ce mama ca"}}},
    {TEST_FILE_6_BEFORE,
     TEST_FILE_6_AFTER,
     "x",
     "text node with element with no children",
     REPLACEMENT_TEXT_NODE,
     {{"my special text plan"}}},
    {TEST_FILE_7_BEFORE,
     TEST_FILE_7_AFTER,
     "body",
     "string with element with multiple children",
     REPLACEMENT_FROM_STRING,
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
     "string with element with 1 child",
     REPLACEMENT_FROM_STRING,
     {{"<whoop></whoop>"}}},
    {TEST_FILE_9_BEFORE,
     TEST_FILE_9_AFTER,
     "x",
     "string with element with no children",
     REPLACEMENT_FROM_STRING,
     {{"text only gang"}}},
    {TEST_FILE_10_BEFORE,
     TEST_FILE_10_AFTER,
     "x",
     "string double text merge in child element",
     REPLACEMENT_FROM_STRING,
     {{"at the start<h1></h1><h2></h2>at the end"}}},
    {TEST_FILE_11_BEFORE,
     TEST_FILE_11_AFTER,
     "x",
     "string double text merge in root element",
     REPLACEMENT_FROM_STRING,
     {{"at the start<h1></h1><h2></h2>at the end"}}},
    {TEST_FILE_12_BEFORE,
     TEST_FILE_12_AFTER,
     "x",
     "string single top merge in root element",
     REPLACEMENT_FROM_STRING,
     {{"at the start<h1></h1><h2></h2>at the end"}}},
};
static const size_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus testReplacements(const char *fileLocation1,
                                   const char *fileLocation2,
                                   const char *cssQuery,
                                   const ReplacementType replacementType,
                                   const ReplacementInput *replacementInput) {
    TestStatus result = TEST_FAILURE;

    ComparisonTest comparisonTest;
    result = initComparisonTest(&comparisonTest, fileLocation1, fileLocation2);
    if (result != TEST_SUCCESS) {
        return result;
    }

    node_id foundNode = 0;
    if (cssQuery != NULL) {
        result =
            getNodeFromQuerySelector(cssQuery, &comparisonTest, &foundNode);
    }
    if (result != TEST_SUCCESS) {
        return result;
    }

    DomStatus domStatus = DOM_SUCCESS;
    switch (replacementType) {
    case REPLACEMENT_DOCUMENT_NODE: {
        domStatus = replaceWithDocumentNode(
            foundNode, &replacementInput->documentNode,
            &comparisonTest.startDom, &comparisonTest.startDataContainer);
        break;
    }
    case REPLACEMENT_TEXT_NODE: {
        domStatus = replaceWithTextNode(foundNode, replacementInput->text,
                                        &comparisonTest.startDom,
                                        &comparisonTest.startDataContainer);
        break;
    }
    case REPLACEMENT_FROM_STRING: {
        domStatus = replaceWithNodesFromString(
            foundNode, replacementInput->text, &comparisonTest.startDom,
            &comparisonTest.startDataContainer);
        break;
    }
    default: {
        return failWithMessage("No suitable replacement type was supplied!\n",
                               &comparisonTest);
    }
    }

    if (domStatus != DOM_SUCCESS) {
        return failWithMessage("Failed to replace node!\n", &comparisonTest);
    }

    return compareAndEndTest(&comparisonTest);
}

bool testDomReplacements(size_t *successes, size_t *failures) {
    printTestTopicStart("DOM replacements");

    size_t localSuccesses = 0;
    size_t localFailures = 0;

    for (size_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];
        printTestStart(testFile.testName);

        if (testReplacements(testFile.fileLocation1, testFile.fileLocation2,
                             testFile.cssQuery, testFile.replacementType,
                             &testFile.replacementInput) != TEST_SUCCESS) {
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
