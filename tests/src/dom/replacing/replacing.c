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
    const flo_html_DocumentNode documentNode;
    const char *text;
} ReplacementInput;

typedef struct {
    const char *fileLocation1;
    const char *fileLocation2;
    const flo_html_String cssQuery;
    const char *testName;
    const ReplacementType replacementType;
    const ReplacementInput replacementInput;
} TestFile;

static const TestFile testFiles[] = {
    {TEST_FILE_1_BEFORE,
     TEST_FILE_1_AFTER,
     FLO_HTML_S("body"),
     "document node with element with multiple children",
     REPLACEMENT_DOCUMENT_NODE,
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
     "document node with element with 1 child",
     REPLACEMENT_DOCUMENT_NODE,
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
     "document node with element with no children",
     REPLACEMENT_DOCUMENT_NODE,
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
     "text node with element with multiple children",
     REPLACEMENT_TEXT_NODE,
     {{FLO_HTML_S("zoinks")}}},
    {TEST_FILE_5_BEFORE,
     TEST_FILE_5_AFTER,
     FLO_HTML_S("div[special-one]"),
     "text node with element with 1 child",
     REPLACEMENT_TEXT_NODE,
     {{FLO_HTML_S("mama ce mama ca")}}},
    {TEST_FILE_6_BEFORE,
     TEST_FILE_6_AFTER,
     FLO_HTML_S("x"),
     "text node with element with no children",
     REPLACEMENT_TEXT_NODE,
     {{FLO_HTML_S("my special text plan")}}},
    {TEST_FILE_7_BEFORE,
     TEST_FILE_7_AFTER,
     FLO_HTML_S("body"),
     "string with element with multiple children",
     REPLACEMENT_FROM_STRING,
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
     "string with element with 1 child",
     REPLACEMENT_FROM_STRING,
     {{FLO_HTML_S("<whoop></whoop>")}}},
    {TEST_FILE_9_BEFORE,
     TEST_FILE_9_AFTER,
     FLO_HTML_S("x"),
     "string with element with no children",
     REPLACEMENT_FROM_STRING,
     {{FLO_HTML_S("text only gang")}}},
    {TEST_FILE_10_BEFORE,
     TEST_FILE_10_AFTER,
     FLO_HTML_S("x"),
     "string double text merge in child element",
     REPLACEMENT_FROM_STRING,
     {{FLO_HTML_S("at the start<h1></h1><h2></h2>at the end")}}},
    {TEST_FILE_11_BEFORE,
     TEST_FILE_11_AFTER,
     FLO_HTML_S("x"),
     "string double text merge in root element",
     REPLACEMENT_FROM_STRING,
     {{FLO_HTML_S("at the start<h1></h1><h2></h2>at the end")}}},
    {TEST_FILE_12_BEFORE,
     TEST_FILE_12_AFTER,
     FLO_HTML_S("x"),
     "string single top merge in root element",
     REPLACEMENT_FROM_STRING,
     {{FLO_HTML_S("at the start<h1></h1><h2></h2>at the end")}}},
};
static const ptrdiff_t numTestFiles = sizeof(testFiles) / sizeof(testFiles[0]);

static TestStatus testReplacements(const flo_html_String fileLocation1,
                                   const flo_html_String fileLocation2,
                                   const flo_html_String cssQuery,
                                   const ReplacementType replacementType,
                                   const ReplacementInput *replacementInput,
                                   flo_html_Arena scratch) {
    TestStatus result = TEST_FAILURE;

    ComparisonTest comparisonTest =
        initComparisonTest(fileLocation1, fileLocation2, &scratch);
    if (result != TEST_SUCCESS) {
        return result;
    }

    flo_html_node_id foundNode = 0;
    if (cssQuery.len > 0) {
        result = getNodeFromQuerySelector(cssQuery, &comparisonTest, &foundNode,
                                          scratch);
    }
    if (result != TEST_SUCCESS) {
        return result;
    }

    flo_html_DomStatus domStatus = DOM_SUCCESS;
    switch (replacementType) {
    case REPLACEMENT_DOCUMENT_NODE: {
        domStatus = flo_html_replaceWithDocumentNode(
            foundNode, &replacementInput->documentNode, comparisonTest.actual,
            &scratch);
        break;
    }
    case REPLACEMENT_TEXT_NODE: {
        domStatus = flo_html_replaceWithTextNode(
            foundNode,
            FLO_HTML_S_LEN(replacementInput->text,
                           strlen(replacementInput->text)),
            comparisonTest.actual, &scratch);
        break;
    }
    case REPLACEMENT_FROM_STRING: {
        domStatus = flo_html_replaceWithHTMLFromString(
            foundNode,
            FLO_HTML_S_LEN(replacementInput->text,
                           strlen(replacementInput->text)),
            comparisonTest.actual, &scratch);
        break;
    }
    default: {
        return failWithMessage(
            FLO_HTML_S("No suitable replacement type was supplied!\n"));
    }
    }

    if (domStatus != DOM_SUCCESS) {
        return failWithMessage(FLO_HTML_S("Failed to replace node!\n"));
    }

    return compareAndEndTest(&comparisonTest, scratch);
}

bool testflo_html_DomReplacements(ptrdiff_t *successes, ptrdiff_t *failures,
                                  flo_html_Arena scratch) {
    printTestTopicStart("DOM replacements");

    ptrdiff_t localSuccesses = 0;
    ptrdiff_t localFailures = 0;

    for (ptrdiff_t i = 0; i < numTestFiles; i++) {
        TestFile testFile = testFiles[i];
        printTestStart(testFile.testName);

        if (testReplacements(FLO_HTML_S_LEN(testFile.fileLocation1,
                                            strlen(testFile.fileLocation1)),
                             FLO_HTML_S_LEN(testFile.fileLocation2,
                                            strlen(testFile.fileLocation2)),
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
