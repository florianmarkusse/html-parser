#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/parser.h"
#include "flo/html-parser/util/array.h"
#include "flo/html-parser/util/file/read.h"
#include "flo/html-parser/util/memory.h"

flo_html_Dom *flo_html_createDomFromFile(flo_html_String fileLocation,
                                         flo_html_Arena *perm) {
    flo_html_String content;
    flo_html_FileStatus fileStatus =
        flo_html_readFile(fileLocation, &content, perm);
    if (fileStatus != FILE_SUCCESS) {
        FLO_HTML_ERROR_WITH_CODE_FORMAT(flo_html_fileStatusToString(fileStatus),
                                        "Failed to read file: \"%s\"",
                                        fileLocation.buf);
        return NULL;
    }

    return flo_html_createDom(content, perm);
}

flo_html_Dom *flo_html_createDom(flo_html_String htmlString,
                                 flo_html_Arena *perm) {
    flo_html_Dom *result =
        FLO_HTML_NEW(perm, flo_html_Dom, 1, FLO_HTML_ZERO_MEMORY);

    *FLO_HTML_PUSH(&result->nodes, perm) = (flo_html_Node){
        .nodeID = FLO_HTML_ERROR_NODE_ID,
        .nodeType = NODE_TYPE_ERROR,
    };
    *FLO_HTML_PUSH(&result->nodes, perm) = (flo_html_Node){
        .nodeID = FLO_HTML_ROOT_NODE_ID,
        .nodeType = NODE_TYPE_ROOT,
    };

    *FLO_HTML_PUSH(&result->tagRegistry, perm) = (flo_html_TagRegistration){0};
    *FLO_HTML_PUSH(&result->boolPropRegistry, perm) = (flo_html_HashElement){0};
    *FLO_HTML_PUSH(&result->propKeyRegistry, perm) = (flo_html_HashElement){0};
    *FLO_HTML_PUSH(&result->propValueRegistry, perm) =
        (flo_html_HashElement){0};

    result->tags =
        flo_html_initStringHashSet(FLO_HTML_REGISTRY_START_SIZE, perm),
    result->boolPropsSet =
        flo_html_initStringHashSet(FLO_HTML_REGISTRY_START_SIZE, perm),
    result->propKeys =
        flo_html_initStringHashSet(FLO_HTML_REGISTRY_START_SIZE, perm),
    result->propValues =
        flo_html_initStringHashSet(FLO_HTML_REGISTRY_START_SIZE, perm),

    flo_html_parseRoot(htmlString, result, perm);

    return result;
}
