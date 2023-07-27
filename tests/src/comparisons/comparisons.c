#include "comparisons/comparisons.h"
#include "dom/document-comparison.h"
#include "dom/document-user.h"
#include "dom/document.h"
#include "test-status.h"
#include "utils/print/error.h"

#define CURRENT_DIR "tests/src/comparisons/"
#define TEST_1_FILE CURRENT_DIR "test-1.html"
#define TEST_2_FILE CURRENT_DIR "test-2.html"

TestStatus testDifferent() {
    createGlobals();

    Document doc1;
    if (createFromFile(TEST_1_FILE, &doc1) != DOCUMENT_SUCCESS) {
        destroyGlobals();
        return 1;
    }

    Document doc2;
    if (createFromFile(TEST_2_FILE, &doc2) != DOCUMENT_SUCCESS) {
        destroyGlobals();
        return;
    }

    ComparisonStatus comp = equals(&doc1, &doc2);

    ERROR_WITH_CODE_ONLY(comparisonStatusToString(comp),
                         "The comparison result");

    destroyDocument(&doc1);
    destroyDocument(&doc2);

    destroyGlobals();
}

void testComparisons() {
    createGlobals();

    Document doc1;
    if (createFromFile(TEST_1_FILE, &doc1) != DOCUMENT_SUCCESS) {
        destroyGlobals();
        return;
    }

    Document doc2;
    if (createFromFile(TEST_2_FILE, &doc2) != DOCUMENT_SUCCESS) {
        destroyGlobals();
        return;
    }

    ComparisonStatus comp = equals(&doc1, &doc2);

    ERROR_WITH_CODE_ONLY(comparisonStatusToString(comp),
                         "The comparison result");

    destroyDocument(&doc1);
    destroyDocument(&doc2);

    destroyGlobals();
}
