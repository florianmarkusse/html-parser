#include "flo/html-parser/dom/dom.h"
#include "array.h"
#include "file/read.h"
#include "flo/html-parser/parser.h"
#include "memory.h"

flo_html_Dom *flo_html_createDomFromFile(flo_String fileLocation,
                                         flo_Arena *perm) {
    flo_String content;
    flo_FileStatus fileStatus = flo_readFile(fileLocation, &content, perm);
    if (fileStatus != FILE_SUCCESS) {
        FLO_ERROR_WITH_CODE_FORMAT(flo_fileStatusToString(fileStatus),
                                   "Failed to read file: \"%s\"",
                                   fileLocation.buf);
        return NULL;
    }

    return flo_html_createDom(content, perm);
}

flo_html_Dom *flo_html_createDom(flo_String htmlString, flo_Arena *perm) {
    flo_html_Dom *result = FLO_NEW(perm, flo_html_Dom, 1, FLO_ZERO_MEMORY);

    *FLO_PUSH(&result->nodes, perm) = (flo_html_Node){
        .nodeID = FLO_HTML_ERROR_NODE_ID,
        .nodeType = NODE_TYPE_ERROR,
    };
    *FLO_PUSH(&result->nodes, perm) = (flo_html_Node){
        .nodeID = FLO_HTML_ROOT_NODE_ID,
        .nodeType = NODE_TYPE_ROOT,
    };

    // Insert empty values to start with because otherwise we would need to
    // constantly do ID - 1.
    *FLO_PUSH(&result->tagRegistry, perm) = (flo_html_TagRegistration){0};
    *FLO_PUSH(&result->boolPropRegistry, perm) = FLO_EMPTY_STRING;
    *FLO_PUSH(&result->propKeyRegistry, perm) = FLO_EMPTY_STRING;
    *FLO_PUSH(&result->propValueRegistry, perm) = FLO_EMPTY_STRING;

    result->tags =
        flo_initStringHashSet(FLO_HTML_REGISTRY_START_SIZE, perm);
    result->boolPropsSet =
        flo_initStringHashSet(FLO_HTML_REGISTRY_START_SIZE, perm);
    result->propKeys =
        flo_initStringHashSet(FLO_HTML_REGISTRY_START_SIZE, perm);
    result->propValues =
        flo_initStringHashSet(FLO_HTML_REGISTRY_START_SIZE, perm);

    return flo_html_parseRoot(htmlString, result, perm);
}

flo_html_Dom *flo_html_duplicateDom(flo_html_Dom *dom, flo_Arena *perm) {
    flo_html_Dom *result = FLO_NEW(perm, flo_html_Dom, 1);

    FLO_COPY_DYNAMIC_ARRAY(result->nodes, dom->nodes, flo_html_Node, perm);
    FLO_COPY_DYNAMIC_ARRAY(result->parentFirstChilds, dom->parentFirstChilds,
                           flo_html_ParentChild, perm);
    FLO_COPY_DYNAMIC_ARRAY(result->parentChilds, dom->parentChilds,
                           flo_html_ParentChild, perm);
    FLO_COPY_DYNAMIC_ARRAY(result->nextNodes, dom->nextNodes, flo_html_NextNode,
                           perm);
    FLO_COPY_DYNAMIC_ARRAY(result->boolProps, dom->boolProps,
                           flo_html_BooleanProperty, perm);
    FLO_COPY_DYNAMIC_ARRAY(result->props, dom->props, flo_html_Property, perm);

    FLO_COPY_DYNAMIC_ARRAY(result->tagRegistry, dom->tagRegistry,
                           flo_html_TagRegistration, perm);
    FLO_COPY_DYNAMIC_ARRAY(result->boolPropRegistry, dom->boolPropRegistry,
                           flo_String, perm);
    FLO_COPY_DYNAMIC_ARRAY(result->propKeyRegistry, dom->propKeyRegistry,
                           flo_String, perm);
    FLO_COPY_DYNAMIC_ARRAY(result->propValueRegistry, dom->propValueRegistry,
                           flo_String, perm);

    result->tags = flo_copyStringHashSet(&dom->tags, perm);
    result->boolPropsSet = flo_copyStringHashSet(&dom->boolPropsSet, perm);
    result->propKeys = flo_copyStringHashSet(&dom->propKeys, perm);
    result->propValues = flo_copyStringHashSet(&dom->propValues, perm);

    return result;
}
