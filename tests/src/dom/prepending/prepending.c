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
    flo_html_DocumentNode documentNode;
    char *text;
} PrependInput;

typedef struct {
    char *fileLocation1;
    char *fileLocation2;
    flo_html_String cssQuery;
    char *testName;
    PrependType prependType;
    PrependInput prependInput;
} TestFile;

static TestFile testFiles[] = {
    {TEST_FILE_1_BEFORE,
     TEST_FILE_1_AFTER,
     FLO_HTML_S("body"),
     "document node to element with multiple children",
     PREPEND_DOCUMENT_NODE,
     {{FLO_HTML_S("example-tag"),
       true,
       {FLO_HTML_S("prop1"), FLO_HTML_S("prop2")},
       2,
       {FLO_HTML_S("key1"), FLO_HTML_S("key2")},
       {FLO_HTML_S("value1"), FLO_HTML_S("value2")},
       2}}},
    {TEST_FILE_2_BEFORE,
     TEST_FILE_2_AFTER,
     FLO_HTML_S("div[special-one]"),
     "document node to element with 1 child",
     PREPEND_DOCUMENT_NODE,
     {{FLO_HTML_S("example-tag"),
       false,
       {},
       0,
       {FLO_HTML_S("bla-bla")},
       {FLO_HTML_S("bla-bla")},
       1}}},
    {TEST_FILE_3_BEFORE,
     TEST_FILE_3_AFTER,
     FLO_HTML_S("x"),
     "document node to element with no children",
     PREPEND_DOCUMENT_NODE,
     {{FLO_HTML_S("example-tag"),
       true,
       {FLO_HTML_S("required"), FLO_HTML_S("help-me")},
       2,
       {},
       {},
       0}}},
    {TEST_FILE_4_BEFORE,
     TEST_FILE_4_AFTER,
     FLO_HTML_S("body"),
     "text node to element with multiple children",
     PREPEND_TEXT_NODE,
     {{FLO_HTML_S("zoinks")}}},
    {TEST_FILE_5_BEFORE,
     TEST_FILE_5_AFTER,
     FLO_HTML_S("div[special-one]"),
     "text node to element with 1 child",
     PREPEND_TEXT_NODE,
     {{FLO_HTML_S("mama ce mama ca")}}},
    {TEST_FILE_6_BEFORE,
     TEST_FILE_6_AFTER,
     FLO_HTML_S("x"),
     "text node to element with no children",
     PREPEND_TEXT_NODE,
     {{FLO_HTML_S("my special text plan")}}},
    {TEST_FILE_7_BEFORE,
     TEST_FILE_7_AFTER,
     FLO_HTML_S("body"),
     "string to element with multiple children",
     PREPEND_FROM_STRING,
     {{FLO_HTML_S("<body style=\"newstyle\">"
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
                  "</body>")}}},
    {TEST_FILE_8_BEFORE,
     TEST_FILE_8_AFTER,
     FLO_HTML_S("div[special-one]"),
     "string to element with 1 child",
     PREPEND_FROM_STRING,
     {{FLO_HTML_S("<whoop></whoop>")}}},
    {TEST_FILE_9_BEFORE,
     TEST_FILE_9_AFTER,
     FLO_HTML_S("x"),
     "string to element with no children",
     PREPEND_FROM_STRING,
     {{FLO_HTML_S("text only gang")}}},
    {TEST_FILE_10_BEFORE,
     TEST_FILE_10_AFTER,
     FLO_HTML_EMPTY_STRING,
     "string to root",
     PREPEND_FROM_STRING,
     {{FLO_HTML_S("<h1></h1><h2></h2>")}}},
    {TEST_FILE_11_BEFORE,
     TEST_FILE_11_AFTER,
     FLO_HTML_S("body"),
     "string merge with first child",
     PREPEND_FROM_STRING,
     {{FLO_HTML_S("new text here<h1></h1><p></p>I got prepended to")}}},
    {TEST_FILE_12_BEFORE,
     TEST_FILE_12_AFTER,
     FLO_HTML_EMPTY_STRING,
     "string merge with first root element",
     PREPEND_FROM_STRING,
     {{FLO_HTML_S("<html></html><wtf></wtf>bottom text")}}},
    {TEST_FILE_13_BEFORE,
     TEST_FILE_13_AFTER,
     FLO_HTML_EMPTY_STRING,
     "text node merge with first root element",
     PREPEND_TEXT_NODE,
     {{FLO_HTML_S("even more")}}},
};
static ptrdiff_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus testPrependix(flo_html_String fileLocation1,
                                flo_html_String fileLocation2,
                                flo_html_String cssQuery,
                                PrependType prependType,
                                PrependInput *prependInput,
                                flo_html_Arena scratch) {
    ComparisonTest comparisonTest =
        initComparisonTest(fileLocation1, fileLocation2, &scratch);

    TestStatus result = TEST_FAILURE;
    flo_html_node_id foundNode = FLO_HTML_ROOT_NODE_ID;
    if (cssQuery.len > 0) {
        result = getNodeFromQuerySelector(cssQuery, &comparisonTest, &foundNode,
                                          scratch);
        if (result != TEST_SUCCESS) {
            return failWithMessageAndCode(
                FLO_HTML_S("Failed to get node from DOM!\n"), result);
        }
    }

    flo_html_node_id prependedNodeID = FLO_HTML_ERROR_NODE_ID;
    switch (prependType) {
    case PREPEND_DOCUMENT_NODE: {
        prependedNodeID =
            flo_html_prependDocumentNode(foundNode, &prependInput->documentNode,
                                         comparisonTest.actual, &scratch);
        break;
    }
    case PREPEND_TEXT_NODE: {
        prependedNodeID = flo_html_prependTextNode(
            foundNode,
            FLO_HTML_S_LEN(prependInput->text, strlen(prependInput->text)),
            comparisonTest.actual, &scratch);
        break;
    }
    case PREPEND_FROM_STRING: {
        prependedNodeID = flo_html_prependHTMLFromString(
            foundNode,
            FLO_HTML_S_LEN(prependInput->text, strlen(prependInput->text)),
            comparisonTest.actual, &scratch);
        break;
    }
    default: {
        return failWithMessage(
            FLO_HTML_S("No suitable prependix type was supplied!\n"));
    }
    }

    if (prependedNodeID == FLO_HTML_ERROR_NODE_ID) {
        return failWithMessage(
            FLO_HTML_S("Failed to prepend document to node!\n"));
    }

    return compareAndEndTest(&comparisonTest, scratch);
}

bool testflo_html_DomPrependices(ptrdiff_t *successes, ptrdiff_t *failures,
                                 flo_html_Arena scratch) {
    printTestTopicStart("DOM prependices");

    ptrdiff_t localSuccesses = 0;
    ptrdiff_t localFailures = 0;

    for (ptrdiff_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];
        printTestStart(testFile.testName);

        if (testPrependix(FLO_HTML_S_LEN(testFile.fileLocation1,
                                         strlen(testFile.fileLocation1)),
                          FLO_HTML_S_LEN(testFile.fileLocation2,
                                         strlen(testFile.fileLocation2)),
                          testFile.cssQuery, testFile.prependType,
                          &testFile.prependInput, scratch) != TEST_SUCCESS) {
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
