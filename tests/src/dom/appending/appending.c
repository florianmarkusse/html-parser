#include <flo/html-parser.h>
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
#define TEST_FILE_10_BEFORE CURRENT_DIR "test-10-before.html"
#define TEST_FILE_10_AFTER CURRENT_DIR "test-10-after.html"
#define TEST_FILE_11_BEFORE CURRENT_DIR "test-11-before.html"
#define TEST_FILE_11_AFTER CURRENT_DIR "test-11-after.html"
#define TEST_FILE_12_BEFORE CURRENT_DIR "test-12-before.html"
#define TEST_FILE_12_AFTER CURRENT_DIR "test-12-after.html"
#define TEST_FILE_13_BEFORE CURRENT_DIR "test-13-before.html"
#define TEST_FILE_13_AFTER CURRENT_DIR "test-13-after.html"

typedef enum {
    APPEND_DOCUMENT_NODE,
    APPEND_TEXT_NODE,
    APPEND_FROM_STRING,
    NUM_APPEND_TYPES
} AppendType;

typedef union {
    const flo_html_DocumentNode documentNode;
    const flo_html_String text;
} AppendInput;

typedef struct {
    const char *fileLocation1;
    const char *fileLocation2;
    const flo_html_String cssQuery;
    const char *testName;
    const AppendType appendType;
    const AppendInput appendInput;
} TestFile;

static const TestFile testFiles[] = {
    {TEST_FILE_1_BEFORE,
     TEST_FILE_1_AFTER,
     FLO_HTML_S("body"),
     "document node to element with multiple children",
     APPEND_DOCUMENT_NODE,
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
     APPEND_DOCUMENT_NODE,
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
     APPEND_DOCUMENT_NODE,
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
     APPEND_TEXT_NODE,
     {{FLO_HTML_S("zoinks")}}},
    {TEST_FILE_5_BEFORE,
     TEST_FILE_5_AFTER,
     FLO_HTML_S("div[special-one]"),
     "text node to element with 1 child",
     APPEND_TEXT_NODE,
     {{FLO_HTML_S("mama ce mama ca")}}},
    {TEST_FILE_6_BEFORE,
     TEST_FILE_6_AFTER,
     FLO_HTML_S("x"),
     "text node to element with no children",
     APPEND_TEXT_NODE,
     {{FLO_HTML_S("my special text plan")}}},
    {TEST_FILE_7_BEFORE,
     TEST_FILE_7_AFTER,
     FLO_HTML_S("body"),
     "string to element with multiple children",
     APPEND_FROM_STRING,
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
     APPEND_FROM_STRING,
     {{FLO_HTML_S("<whoop></whoop><ba></ba>")}}},
    {TEST_FILE_9_BEFORE,
     TEST_FILE_9_AFTER,
     FLO_HTML_S("x"),
     "string to element with no children",
     APPEND_FROM_STRING,
     {{FLO_HTML_S("text only gang")}}},
    {TEST_FILE_10_BEFORE,
     TEST_FILE_10_AFTER,
     FLO_HTML_EMPTY_STRING,
     "string to root",
     APPEND_FROM_STRING,
     {{FLO_HTML_S("<h1></h1><h2></h2>")}}},
    {TEST_FILE_11_BEFORE,
     TEST_FILE_11_AFTER,
     FLO_HTML_S("body"),
     "string merge with last child",
     APPEND_FROM_STRING,
     {{FLO_HTML_S("is what I added <h1></h1>Hi there<p></p>")}}},
    {TEST_FILE_12_BEFORE,
     TEST_FILE_12_AFTER,
     FLO_HTML_EMPTY_STRING,
     "string merge with last root element",
     APPEND_FROM_STRING,
     {{FLO_HTML_S(
         "at the end<h1>With an h1</h1><p></p>with more ending text")}}},
    {TEST_FILE_13_BEFORE,
     TEST_FILE_13_AFTER,
     FLO_HTML_S("body"),
     "text node merge to last child",
     APPEND_TEXT_NODE,
     {{FLO_HTML_S("is all I do")}}},
};

static const size_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus testAppendix(const flo_html_String fileLocation1,
                               const flo_html_String fileLocation2,
                               const flo_html_String cssQuery,
                               const AppendType appendType,
                               const AppendInput *appendInput) {
    TestStatus result = TEST_FAILURE;

    ComparisonTest comparisonTest;
    result = initComparisonTest(&comparisonTest, fileLocation1, fileLocation2);
    if (result != TEST_SUCCESS) {
        return result;
    }

    flo_html_node_id foundNode = 0;
    if (cssQuery.len > 0) {
        result =
            getNodeFromQuerySelector(cssQuery, &comparisonTest, &foundNode);
        if (result != TEST_SUCCESS) {
            return result;
        }
    }

    flo_html_DomStatus domStatus = DOM_SUCCESS;
    switch (appendType) {
    case APPEND_DOCUMENT_NODE: {
        domStatus = flo_html_appendDocumentNode(
            foundNode, &appendInput->documentNode,
            &comparisonTest.startflo_html_Dom, &comparisonTest.startTextStore);
        break;
    }
    case APPEND_TEXT_NODE: {
        domStatus = flo_html_appendTextNode(foundNode, appendInput->text,
                                            &comparisonTest.startflo_html_Dom,
                                            &comparisonTest.startTextStore);
        break;
    }
    case APPEND_FROM_STRING: {
        domStatus = flo_html_appendHTMLFromString(
            foundNode, appendInput->text, &comparisonTest.startflo_html_Dom,
            &comparisonTest.startTextStore);
        break;
    }
    default: {
        return failWithMessage(
            FLO_HTML_S("No suitable appendix type was supplied!\n"),
            &comparisonTest);
    }
    }

    if (domStatus != DOM_SUCCESS) {
        return failWithMessage(
            FLO_HTML_S("Failed to append document to node!\n"),
            &comparisonTest);
    }

    return compareAndEndTest(&comparisonTest);
}

bool testflo_html_DomAppendices(size_t *successes, size_t *failures) {
    printTestTopicStart("DOM appendices");

    size_t localSuccesses = 0;
    size_t localFailures = 0;

    for (size_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];
        printTestStart(testFile.testName);

        if (testAppendix(FLO_HTML_S_LEN(testFile.fileLocation1,
                                        strlen(testFile.fileLocation1)),
                         FLO_HTML_S_LEN(testFile.fileLocation2,
                                        strlen(testFile.fileLocation2)),
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
