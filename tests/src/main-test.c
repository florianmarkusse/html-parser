#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "dom/document-comparison.h"
#include "dom/document-user.h"
#include "dom/document-writing.h"
#include "dom/document.h"
#include "utils/file/read.h"
#include "utils/print/error.h"

static const char *const test1File = "tests/data/html/test-1.html";
static const char *const test2File = "tests/data/html/test-2.html";

int main() {
    createGlobals();

    Document doc1;
    if (createFromFile(test1File, &doc1) != DOCUMENT_SUCCESS) {
        destroyGlobals();
        return 1;
    }

    //    printDocumentStatus(&doc1);
    printXML(&doc1);
    writeXMLToFile(&doc1, "tests/data/html/test-1-write.html");

    Document doc2;
    if (createFromFile(test2File, &doc2) != DOCUMENT_SUCCESS) {
        destroyGlobals();
        return 1;
    }

    //    printDocumentStatus(&doc2);
    printXML(&doc2);
    writeXMLToFile(&doc2, "tests/data/html/test-2-write.html");

    ComparisonStatus comp = equals(&doc1, &doc2);

    ERROR_WITH_CODE_ONLY(comparisonStatusToString(comp),
                         "The comparison result");

    destroyDocument(&doc1);
    destroyDocument(&doc2);

    //     printGlobalTagStatus();
    //     printGlobalAttributeStatus();
    //     printGlobalTextStatus();
    //     destroyGlobals();
}
