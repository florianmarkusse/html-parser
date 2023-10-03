#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/parser/parser.h"
#include "flo/html-parser/utils/file/read.h"
#include "flo/html-parser/utils/memory/memory.h"

void initflo_html_BasicRegistry(flo_html_BasicRegistry *basicRegistry,
                                const flo_html_Registration *initRegistration) {
    basicRegistry->registry = malloc(FLO_HTML_PROP_REGISTRY_PAGE_SIZE);
    basicRegistry->registry[0] = *initRegistration;
    basicRegistry->len =
        1; // Start at 1 so we don't need to do tagRegistry[x - 1]
    basicRegistry->cap = FLO_HTML_PROP_REGISTRATIONS_PER_PAGE;
}

flo_html_DomStatus
flo_html_createDomFromFile(const flo_html_String fileLocation,
                           flo_html_Dom *dom, flo_html_TextStore *textStore) {
    flo_html_String content;
    flo_html_FileStatus fileStatus = flo_html_readFile(fileLocation, &content);
    if (fileStatus != FILE_SUCCESS) {
        FLO_HTML_ERROR_WITH_CODE_FORMAT(flo_html_fileStatusToString(fileStatus),
                                        "Failed to read file: \"%s\"",
                                        fileLocation.buf);
        return DOM_ERROR_MEMORY;
    }

    flo_html_DomStatus documentStatus =
        flo_html_createDom(content, dom, textStore);
    if (documentStatus != DOM_SUCCESS) {
        FLO_HTML_FREE_TO_NULL(content.buf);
        FLO_HTML_ERROR_WITH_CODE_FORMAT(
            documentStatusToString(documentStatus),
            "Failed to create document from file \"%s\"", fileLocation.buf);
        printf(" in hereffjdfd\n");
        return documentStatus;
    }
    FLO_HTML_FREE_TO_NULL(content.buf);

    return DOM_SUCCESS;
}

flo_html_DomStatus flo_html_createDom(const flo_html_String htmlString,
                                      flo_html_Dom *dom,
                                      flo_html_TextStore *textStore) {
    dom->firstNodeID = 0;

    flo_html_TagRegistration initTag = {0};
    flo_html_Registration initRegistration = {0};

    dom->tagRegistry = malloc(FLO_HTML_TAG_REGISTRY_PAGE_SIZE);
    dom->tagRegistry[0] = initTag;
    dom->tagRegistryLen =
        1; // Start at 1 so we don't need to do tagRegistry[x - 1]
    dom->tagRegistryCap = FLO_HTML_TAG_REGISTRATIONS_PER_PAGE;

    initflo_html_BasicRegistry(&dom->boolPropRegistry, &initRegistration);
    initflo_html_BasicRegistry(&dom->propKeyRegistry, &initRegistration);
    initflo_html_BasicRegistry(&dom->propValueRegistry, &initRegistration);

    dom->nodes = malloc(FLO_HTML_NODES_PAGE_SIZE);
    flo_html_Node errorNode;
    errorNode.nodeID = 0;
    errorNode.nodeType = NODE_TYPE_ERROR;
    errorNode.tagID = 0;
    dom->nodes[0] = errorNode;

    flo_html_Node rootNode;
    rootNode.nodeID = 1;
    rootNode.nodeType = NODE_TYPE_ROOT;
    rootNode.tagID = 0;
    dom->nodes[1] = rootNode;

    dom->nodeLen = 2; // We start at 2 because 0 is used as error id, and
                      // 1 is used as the root node.
    dom->nodeCap = FLO_HTML_NODES_PER_PAGE;

    dom->parentFirstChilds = malloc(FLO_HTML_PARENT_FIRST_CHILDS_PAGE_SIZE);
    dom->parentFirstChildLen = 0;
    dom->parentFirstChildCap = FLO_HTML_PARENT_FIRST_CHILDS_PER_PAGE;

    dom->parentChilds = malloc(FLO_HTML_PARENT_CHILDS_PAGE_SIZE);
    dom->parentChildLen = 0;
    dom->parentChildCap = FLO_HTML_PARENT_CHILDS_PER_PAGE;

    dom->nextNodes = malloc(NEXT_FLO_HTML_NODES_PAGE_SIZE);
    dom->nextNodeLen = 0;
    dom->nextNodeCap = NEXT_FLO_HTML_NODES_PER_PAGE;

    dom->boolProps = malloc(BOOLEAN_PROPERTIES_PAGE_SIZE);
    dom->boolPropsLen = 0;
    dom->boolPropsCap = BOOLEAN_PROPERTIES_PER_PAGE;

    dom->props = malloc(PROPERTIES_PAGE_SIZE);
    dom->propsLen = 0;
    dom->propsCap = PROPERTIES_PER_PAGE;

    if (dom->nodes == NULL || dom->tagRegistry == NULL ||
        dom->boolPropRegistry.registry == NULL ||
        dom->propKeyRegistry.registry == NULL ||
        dom->propValueRegistry.registry == NULL ||
        dom->parentFirstChilds == NULL || dom->parentChilds == NULL ||
        dom->nextNodes == NULL || dom->boolProps == NULL ||
        dom->props == NULL) {
        FLO_HTML_PRINT_ERROR("Failed to allocate memory for nodes.\n");
        flo_html_destroyDom(dom);
        return DOM_ERROR_MEMORY;
    }

    flo_html_DomStatus domumentStatus =
        flo_html_parseNew(htmlString, dom, textStore);
    if (domumentStatus != DOM_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Failed to parse document.\n");
    }
    return domumentStatus;
}

void flo_html_destroyDom(flo_html_Dom *dom) {
    FLO_HTML_FREE_TO_NULL(dom->nodes);
    FLO_HTML_FREE_TO_NULL(dom->tagRegistry);
    FLO_HTML_FREE_TO_NULL(dom->boolPropRegistry.registry);
    FLO_HTML_FREE_TO_NULL(dom->propKeyRegistry.registry);
    FLO_HTML_FREE_TO_NULL(dom->propValueRegistry.registry);
    FLO_HTML_FREE_TO_NULL(dom->parentFirstChilds);
    FLO_HTML_FREE_TO_NULL(dom->parentChilds);
    FLO_HTML_FREE_TO_NULL(dom->nextNodes);
    FLO_HTML_FREE_TO_NULL(dom->boolProps);
    FLO_HTML_FREE_TO_NULL(dom->props);
}
