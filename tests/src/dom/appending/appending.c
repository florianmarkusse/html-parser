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
    flo_html_DocumentNode documentNode;
    flo_String text;
} AppendInput;

typedef struct {
    char *fileLocation1;
    char *fileLocation2;
    flo_String cssQuery;
    flo_String testName;
    AppendType appendType;
    AppendInput appendInput;
} TestFile;

static TestFile testFiles[] = {
    {TEST_FILE_1_BEFORE,
     TEST_FILE_1_AFTER,
     FLO_STRING("body"),
     FLO_STRING("document node to element with multiple children"),
     APPEND_DOCUMENT_NODE,
     {{FLO_STRING("example-tag"),
       true,
       {FLO_STRING("prop1"), FLO_STRING("prop2")},
       2,
       {FLO_STRING("key1"), FLO_STRING("key2")},
       {FLO_STRING("value1"), FLO_STRING("value2")},
       2}}},
    {TEST_FILE_2_BEFORE,
     TEST_FILE_2_AFTER,
     FLO_STRING("div[special-one]"),
     FLO_STRING("document node to element with 1 child"),
     APPEND_DOCUMENT_NODE,
     {{FLO_STRING("example-tag"),
       false,
       {},
       0,
       {FLO_STRING("bla-bla")},
       {FLO_STRING("bla-bla")},
       1}}},
    {TEST_FILE_3_BEFORE,
     TEST_FILE_3_AFTER,
     FLO_STRING("x"),
     FLO_STRING("document node to element with no children"),
     APPEND_DOCUMENT_NODE,
     {{FLO_STRING("example-tag"),
       true,
       {FLO_STRING("required"), FLO_STRING("help-me")},
       2,
       {},
       {},
       0}}},
    {TEST_FILE_4_BEFORE,
     TEST_FILE_4_AFTER,
     FLO_STRING("body"),
     FLO_STRING("text node to element with multiple children"),
     APPEND_TEXT_NODE,
     {{FLO_STRING("zoinks")}}},
    {TEST_FILE_5_BEFORE,
     TEST_FILE_5_AFTER,
     FLO_STRING("div[special-one]"),
     FLO_STRING("text node to element with 1 child"),
     APPEND_TEXT_NODE,
     {{FLO_STRING("mama ce mama ca")}}},
    {TEST_FILE_6_BEFORE,
     TEST_FILE_6_AFTER,
     FLO_STRING("x"),
     FLO_STRING("text node to element with no children"),
     APPEND_TEXT_NODE,
     {{FLO_STRING("my special text plan")}}},
    {TEST_FILE_7_BEFORE,
     TEST_FILE_7_AFTER,
     FLO_STRING("body"),
     FLO_STRING("string to element with multiple children"),
     APPEND_FROM_STRING,
     {{FLO_STRING("<body style=\"newstyle\">"
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
     FLO_STRING("div[special-one]"),
     FLO_STRING("string to element with 1 child"),
     APPEND_FROM_STRING,
     {{FLO_STRING("<whoop></whoop><ba></ba>")}}},
    {TEST_FILE_9_BEFORE,
     TEST_FILE_9_AFTER,
     FLO_STRING("x"),
     FLO_STRING("string to element with no children"),
     APPEND_FROM_STRING,
     {{FLO_STRING("text only gang")}}},
    {TEST_FILE_10_BEFORE,
     TEST_FILE_10_AFTER,
     FLO_EMPTY_STRING,
     FLO_STRING("string to root"),
     APPEND_FROM_STRING,
     {{FLO_STRING("<h1></h1><h2></h2>")}}},
    {TEST_FILE_11_BEFORE,
     TEST_FILE_11_AFTER,
     FLO_STRING("body"),
     FLO_STRING("string merge with last child"),
     APPEND_FROM_STRING,
     {{FLO_STRING("is what I added <h1></h1>Hi there<p></p>")}}},
    {TEST_FILE_12_BEFORE,
     TEST_FILE_12_AFTER,
     FLO_EMPTY_STRING,
     FLO_STRING("string merge with last root element"),
     APPEND_FROM_STRING,
     {{FLO_STRING(
         "at the end<h1>With an h1</h1><p></p>with more ending text")}}},
    {TEST_FILE_13_BEFORE,
     TEST_FILE_13_AFTER,
     FLO_STRING("body"),
     FLO_STRING("text node merge to last child"),
     APPEND_TEXT_NODE,
     {{FLO_STRING("is all I do")}}},
};

static ptrdiff_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus testAppendix(flo_String fileLocation1,
                               flo_String fileLocation2, flo_String cssQuery,
                               AppendType appendType, AppendInput *appendInput,
                               flo_Arena scratch) {
    ComparisonTest comparisonTest =
        initComparisonTest(fileLocation1, fileLocation2, &scratch);

    TestStatus result = TEST_FAILURE;
    flo_html_node_id foundNode = FLO_HTML_ROOT_NODE_ID;
    if (cssQuery.len > 0) {
        result = getNodeFromQuerySelector(cssQuery, &comparisonTest, &foundNode,
                                          scratch);
        if (result != TEST_SUCCESS) {
            return result;
        }
    }

    flo_html_node_id appendedNodeID = FLO_HTML_ERROR_NODE_ID;
    switch (appendType) {
    case APPEND_DOCUMENT_NODE: {
        appendedNodeID =
            flo_html_appendDocumentNode(foundNode, &appendInput->documentNode,
                                        comparisonTest.actual, &scratch);
        break;
    }
    case APPEND_TEXT_NODE: {
        appendedNodeID = flo_html_appendTextNode(
            foundNode, appendInput->text, comparisonTest.actual, &scratch);
        break;
    }
    case APPEND_FROM_STRING: {
        appendedNodeID = flo_html_appendHTMLFromString(
            foundNode, appendInput->text, comparisonTest.actual, &scratch);
        break;
    }
    default: {
        FLO_LOG_TEST_FAILED {
            FLO_ERROR(
                (FLO_STRING("No suitable appendix type was supplied!\n")));
        }
        return TEST_FAILURE;
    }
    }

    if (appendedNodeID == FLO_HTML_ERROR_NODE_ID) {
        FLO_LOG_TEST_FAILED {
            FLO_ERROR((FLO_STRING("Failed to append node(s) to DOM!\n")));
        }
        return TEST_FAILURE;
    }

    return compareAndEndTest(&comparisonTest, scratch);
}

bool testflo_html_DomAppendices(ptrdiff_t *successes, ptrdiff_t *failures,
                                flo_Arena scratch) {
    printTestTopicStart(FLO_STRING("DOM appendices"));

    ptrdiff_t localSuccesses = 0;
    ptrdiff_t localFailures = 0;

    for (ptrdiff_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];
        printTestStart(testFile.testName);

        if (testAppendix(FLO_STRING_LEN(testFile.fileLocation1,
                                        strlen(testFile.fileLocation1)),
                         FLO_STRING_LEN(testFile.fileLocation2,
                                        strlen(testFile.fileLocation2)),
                         testFile.cssQuery, testFile.appendType,
                         &testFile.appendInput, scratch) != TEST_SUCCESS) {
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
