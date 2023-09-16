#include <flo/html-parser.h>
#include <stdbool.h>
#include <stdio.h>

#include "comparison-test.h"
#include "dom/prepending/prepending.h"
#include "test-status.h"
#include "test.h"

#define CURRENT_DIR "tests/src/dom/prepending/inputs/"
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
#define TEST_FILE_13_BEFORE CURRENT_DIR "test-13-before.html"
#define TEST_FILE_13_AFTER CURRENT_DIR "test-13-after.html"

typedef enum {
    PREPEND_DOCUMENT_NODE,
    PREPEND_TEXT_NODE,
    PREPEND_FROM_STRING,
    NUM_PREPEND_TYPES
} PrependType;

typedef union {
    const DocumentNode documentNode;
    const char *text;
} PrependInput;

typedef struct {
    const char *fileLocation1;
    const char *fileLocation2;
    const char *cssQuery;
    const char *testName;
    const PrependType prependType;
    const PrependInput prependInput;
} __attribute__((aligned(128))) TestFile;

static const TestFile testFiles[] = {
    {TEST_FILE_1_BEFORE,
     TEST_FILE_1_AFTER,
     "body",
     "document node to element with multiple children",
     PREPEND_DOCUMENT_NODE,
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
     PREPEND_DOCUMENT_NODE,
     {{"example-tag", false, {}, 0, {"bla-bla"}, {"bla-bla"}, 1}}},
    {TEST_FILE_3_BEFORE,
     TEST_FILE_3_AFTER,
     "x",
     "document node to element with no children",
     PREPEND_DOCUMENT_NODE,
     {{"example-tag", true, {"required", "help-me"}, 2, {}, {}, 0}}},
    {TEST_FILE_4_BEFORE,
     TEST_FILE_4_AFTER,
     "body",
     "text node to element with multiple children",
     PREPEND_TEXT_NODE,
     {{"zoinks"}}},
    {TEST_FILE_5_BEFORE,
     TEST_FILE_5_AFTER,
     "div[special-one]",
     "text node to element with 1 child",
     PREPEND_TEXT_NODE,
     {{"mama ce mama ca"}}},
    {TEST_FILE_6_BEFORE,
     TEST_FILE_6_AFTER,
     "x",
     "text node to element with no children",
     PREPEND_TEXT_NODE,
     {{"my special text plan"}}},
    {TEST_FILE_7_BEFORE,
     TEST_FILE_7_AFTER,
     "body",
     "string to element with multiple children",
     PREPEND_FROM_STRING,
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
     PREPEND_FROM_STRING,
     {{"<whoop></whoop>"}}},
    {TEST_FILE_9_BEFORE,
     TEST_FILE_9_AFTER,
     "x",
     "string to element with no children",
     PREPEND_FROM_STRING,
     {{"text only gang"}}},
    {TEST_FILE_10_BEFORE,
     TEST_FILE_10_AFTER,
     NULL,
     "string to root",
     PREPEND_FROM_STRING,
     {{"<h1></h1><h2></h2>"}}},
    {TEST_FILE_11_BEFORE,
     TEST_FILE_11_AFTER,
     "body",
     "string merge with first child",
     PREPEND_FROM_STRING,
     {{"new text here<h1></h1><p></p>I got prepended to"}}},
    {TEST_FILE_12_BEFORE,
     TEST_FILE_12_AFTER,
     NULL,
     "string merge with first root element",
     PREPEND_FROM_STRING,
     {{"<html></html><wtf></wtf>bottom text"}}},
    {TEST_FILE_13_BEFORE,
     TEST_FILE_13_AFTER,
     NULL,
     "text node merge with first root element",
     PREPEND_TEXT_NODE,
     {{"even more"}}},
};
static const size_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus testPrependix(const char *fileLocation1,
                                const char *fileLocation2, const char *cssQuery,
                                const PrependType prependType,
                                const PrependInput *prependInput) {
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
    switch (prependType) {
    case PREPEND_DOCUMENT_NODE: {
        domStatus = flo_html_prependDocumentNode(foundNode, &prependInput->documentNode,
                                        &comparisonTest.startDom,
                                        &comparisonTest.startTextStore);
        break;
    }
    case PREPEND_TEXT_NODE: {
        domStatus = flo_html_prependTextNode(foundNode, prependInput->text,
                                    &comparisonTest.startDom,
                                    &comparisonTest.startTextStore);
        break;
    }
    case PREPEND_FROM_STRING: {
        domStatus = flo_html_prependHTMLFromString(foundNode, prependInput->text,
                                           &comparisonTest.startDom,
                                           &comparisonTest.startTextStore);
        break;
    }
    default: {
        return failWithMessage("No suitable prependix type was supplied!\n",
                               &comparisonTest);
    }
    }

    if (domStatus != DOM_SUCCESS) {
        return failWithMessage("Failed to prepend document to node!\n",
                               &comparisonTest);
    }

    return compareAndEndTest(&comparisonTest);
}

bool testDomPrependices(size_t *successes, size_t *failures) {
    printTestTopicStart("DOM prependices");

    size_t localSuccesses = 0;
    size_t localFailures = 0;

    for (size_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];
        printTestStart(testFile.testName);

        if (testPrependix(testFile.fileLocation1, testFile.fileLocation2,
                          testFile.cssQuery, testFile.prependType,
                          &testFile.prependInput) != TEST_SUCCESS) {
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
