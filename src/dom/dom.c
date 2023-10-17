#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/parser.h"
#include "flo/html-parser/util/file/read.h"
#include "flo/html-parser/util/memory.h"

flo_html_BasicRegistry
initflo_html_BasicRegistry(flo_html_HashElement initRegistration,
                           flo_html_Arena *perm) {
    flo_html_BasicRegistry basicRegistry;

    basicRegistry.hashes = FLO_HTML_NEW(perm, flo_html_HashElement,
                                        FLO_HTML_MAX_PROP_REGISTRATIONS);
    basicRegistry.hashes[0] = initRegistration;
    basicRegistry.len =
        1; // Start at 1 so we don't need to do tagRegistry[x - 1]
    basicRegistry.cap = FLO_HTML_MAX_PROP_REGISTRATIONS;

    return basicRegistry;
}

flo_html_DomStatus
flo_html_createDomFromFile(const flo_html_String fileLocation,
                           flo_html_Dom *dom, flo_html_TextStore *textStore,
                           flo_html_Arena *perm) {
    flo_html_String content;
    flo_html_FileStatus fileStatus =
        flo_html_readFile(fileLocation, &content, perm);
    if (fileStatus != FILE_SUCCESS) {
        FLO_HTML_ERROR_WITH_CODE_FORMAT(flo_html_fileStatusToString(fileStatus),
                                        "Failed to read file: \"%s\"",
                                        fileLocation.buf);
        return DOM_ERROR_MEMORY;
    }

    flo_html_createDom(content, dom, textStore, perm);

    return DOM_SUCCESS;
}

void flo_html_createDom(const flo_html_String htmlString, flo_html_Dom *dom,
                        flo_html_TextStore *textStore, flo_html_Arena *perm) {
    flo_html_TagRegistration initTag = {0};
    flo_html_HashElement initHash = {0};

    dom->tagRegistry = FLO_HTML_NEW(perm, flo_html_TagRegistration, 1U << 10U);
    dom->tagRegistry[0] = initTag;
    dom->tagRegistryLen =
        1; // Start at 1 so we don't need to do tagRegistry[x - 1]
    dom->tagRegistryCap = FLO_HTML_MAX_TAG_REGISTRATIONS;

    dom->boolPropRegistry = initflo_html_BasicRegistry(initHash, perm);
    dom->propKeyRegistry = initflo_html_BasicRegistry(initHash, perm);
    dom->propValueRegistry = initflo_html_BasicRegistry(initHash, perm);

    dom->nodes = FLO_HTML_NEW(perm, flo_html_Node, FLO_HTML_MAX_NODES);
    flo_html_Node errorNode;
    errorNode.nodeID = FLO_HTML_ERROR_NODE_ID;
    errorNode.nodeType = NODE_TYPE_ERROR;
    errorNode.tagID = 0;
    dom->nodes[0] = errorNode;

    flo_html_Node rootNode;
    rootNode.nodeID = FLO_HTML_ROOT_NODE_ID;
    rootNode.nodeType = NODE_TYPE_ROOT;
    rootNode.tagID = 0;
    dom->nodes[1] = rootNode;

    dom->nodeLen = 2; // We start at 2 because 0 is used as error id, and
                      // 1 is used as the root node.
    dom->nodeCap = FLO_HTML_MAX_NODES;

    dom->parentFirstChilds = FLO_HTML_NEW(perm, flo_html_ParentChild,
                                          FLO_HTML_MAX_PARENT_FIRST_CHILDS);
    dom->parentFirstChildLen = 0;
    dom->parentFirstChildCap = FLO_HTML_MAX_PARENT_FIRST_CHILDS;

    dom->parentChilds =
        FLO_HTML_NEW(perm, flo_html_ParentChild, FLO_HTML_MAX_PARENT_CHILDS);
    dom->parentChildLen = 0;
    dom->parentChildCap = FLO_HTML_MAX_PARENT_CHILDS;

    dom->nextNodes =
        FLO_HTML_NEW(perm, flo_html_NextNode, FLO_HTML_MAX_NEXT_NODES);
    dom->nextNodeLen = 0;
    dom->nextNodeCap = FLO_HTML_MAX_NEXT_NODES;

    dom->boolProps =
        FLO_HTML_NEW(perm, flo_html_BooleanProperty, FLO_HTML_MAX_BOOL_PROPS);
    dom->boolPropsLen = 0;
    dom->boolPropsCap = FLO_HTML_MAX_BOOL_PROPS;

    dom->props = FLO_HTML_NEW(perm, flo_html_Property, FLO_HTML_MAX_PROPS);
    dom->propsLen = 0;
    dom->propsCap = FLO_HTML_MAX_PROPS;

    flo_html_parseRoot(
        htmlString, (flo_html_ParsedHTML){.dom = dom, .textStore = textStore},
        perm);
}
