#include <flo/html-parser.h>
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
    flo_html_DocumentNode documentNode;
    flo_String text;
} ReplacementInput;

typedef struct {
    char *fileLocation1;
    char *fileLocation2;
    flo_String cssQuery;
    flo_String testName;
    ReplacementType replacementType;
    ReplacementInput replacementInput;
} TestFile;

static TestFile testFiles[] = {
    {TEST_FILE_1_BEFORE,
     TEST_FILE_1_AFTER,
     FLO_STRING("body"),
     FLO_STRING("document node with element with multiple children"),
     REPLACEMENT_DOCUMENT_NODE,
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
     FLO_STRING("document node with element with 1 child"),
     REPLACEMENT_DOCUMENT_NODE,
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
     FLO_STRING("document node with element with no children"),
     REPLACEMENT_DOCUMENT_NODE,
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
     FLO_STRING("text node with element with multiple children"),
     REPLACEMENT_TEXT_NODE,
     {{FLO_STRING("zoinks")}}},
    {TEST_FILE_5_BEFORE,
     TEST_FILE_5_AFTER,
     FLO_STRING("div[special-one]"),
     FLO_STRING("text node with element with 1 child"),
     REPLACEMENT_TEXT_NODE,
     {{FLO_STRING("mama ce mama ca")}}},
    {TEST_FILE_6_BEFORE,
     TEST_FILE_6_AFTER,
     FLO_STRING("x"),
     FLO_STRING("text node with element with no children"),
     REPLACEMENT_TEXT_NODE,
     {{FLO_STRING("my special text plan")}}},
    {TEST_FILE_7_BEFORE,
     TEST_FILE_7_AFTER,
     FLO_STRING("body"),
     FLO_STRING("string with element with multiple children"),
     REPLACEMENT_FROM_STRING,
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
     FLO_STRING("string with element with 1 child"),
     REPLACEMENT_FROM_STRING,
     {{FLO_STRING("<whoop></whoop>")}}},
    {TEST_FILE_9_BEFORE,
     TEST_FILE_9_AFTER,
     FLO_STRING("x"),
     FLO_STRING("string with element with no children"),
     REPLACEMENT_FROM_STRING,
     {{FLO_STRING("text only gang")}}},
    {TEST_FILE_10_BEFORE,
     TEST_FILE_10_AFTER,
     FLO_STRING("x"),
     FLO_STRING("string double text merge in child element"),
     REPLACEMENT_FROM_STRING,
     {{FLO_STRING("at the start<h1></h1><h2></h2>at the end")}}},
    {TEST_FILE_11_BEFORE,
     TEST_FILE_11_AFTER,
     FLO_STRING("x"),
     FLO_STRING("string double text merge in root element"),
     REPLACEMENT_FROM_STRING,
     {{FLO_STRING("at the start<h1></h1><h2></h2>at the end")}}},
    {TEST_FILE_12_BEFORE,
     TEST_FILE_12_AFTER,
     FLO_STRING("x"),
     FLO_STRING("string single top merge in root element"),
     REPLACEMENT_FROM_STRING,
     {{FLO_STRING("at the start<h1></h1><h2></h2>at the end")}}},
};
static ptrdiff_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus testReplacements(char *fileLocation1, char *fileLocation2,
                                   flo_String cssQuery,
                                   ReplacementType replacementType,
                                   ReplacementInput *replacementInput,
                                   flo_Arena scratch) {
    ComparisonTest comparisonTest =
        initComparisonTest(fileLocation1, fileLocation2, &scratch);

    TestStatus result = TEST_FAILURE;
    flo_html_node_id foundNode = FLO_HTML_ROOT_NODE_ID;
    if (cssQuery.len > 0) {
        result = getNodeFromQuerySelector(cssQuery, &comparisonTest, &foundNode,
                                          scratch);
        if (result != TEST_SUCCESS) {
            FLO_LOG_TEST_FAILED {
                FLO_ERROR((FLO_STRING("Failed to get node from DOM!\n")));
            }
            return result;
        }
    }

    flo_html_node_id replacedNodeID = FLO_HTML_ERROR_NODE_ID;
    switch (replacementType) {
    case REPLACEMENT_DOCUMENT_NODE: {
        replacedNodeID = flo_html_replaceWithDocumentNode(
            foundNode, &replacementInput->documentNode, comparisonTest.actual,
            &scratch);
        break;
    }
    case REPLACEMENT_TEXT_NODE: {
        replacedNodeID = flo_html_replaceWithTextNode(
            foundNode, replacementInput->text, comparisonTest.actual, &scratch);
        break;
    }
    case REPLACEMENT_FROM_STRING: {
        replacedNodeID = flo_html_replaceWithHTMLFromString(
            foundNode, replacementInput->text, comparisonTest.actual, &scratch);
        break;
    }
    default: {
        FLO_LOG_TEST_FAILED {
            FLO_ERROR(
                (FLO_STRING("No suitable replacement type was supplied!\n")));
        }
        return TEST_FAILURE;
    }
    }

    if (replacedNodeID == FLO_HTML_ERROR_NODE_ID) {
        FLO_LOG_TEST_FAILED {
            FLO_ERROR((FLO_STRING("Failed to replace node!\n")));
        }
        return TEST_FAILURE;
    }

    return compareAndEndTest(&comparisonTest, scratch);
}

bool testflo_html_DomReplacements(ptrdiff_t *successes, ptrdiff_t *failures,
                                  flo_Arena scratch) {
    printTestTopicStart(FLO_STRING("DOM replacements"));

    ptrdiff_t localSuccesses = 0;
    ptrdiff_t localFailures = 0;

    for (ptrdiff_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];
        printTestStart(testFile.testName);

        if (testReplacements(testFile.fileLocation1, testFile.fileLocation2,
                             testFile.cssQuery, testFile.replacementType,
                             &testFile.replacementInput,
                             scratch) != TEST_SUCCESS) {
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
