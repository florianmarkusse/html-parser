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

flo_html_DomStatus createflo_html_DomFromFile(const char *fileLocation,
                                              flo_html_Dom *dom,
                                              flo_html_TextStore *textStore) {
    char *buffer = NULL;
    flo_html_FileStatus fileStatus = flo_html_readFile(fileLocation, &buffer);
    if (fileStatus != FILE_SUCCESS) {
        FLO_HTML_ERROR_WITH_CODE_FORMAT(flo_html_fileStatusToString(fileStatus),
                                        "Failed to read file: \"%s\"",
                                        fileLocation);
        return DOM_ERROR_MEMORY;
    }

    flo_html_DomStatus documentStatus =
        createflo_html_Dom(buffer, dom, textStore);
    if (documentStatus != DOM_SUCCESS) {
        FLO_HTML_FREE_TO_NULL(buffer);
        FLO_HTML_ERROR_WITH_CODE_FORMAT(
            documentStatusToString(documentStatus),
            "Failed to create document from file \"%s\"", fileLocation);
        return documentStatus;
    }
    FLO_HTML_FREE_TO_NULL(buffer);

    return DOM_SUCCESS;
}

flo_html_DomStatus createflo_html_Dom(const char *htmlString, flo_html_Dom *dom,
                                      flo_html_TextStore *textStore) {
    dom->firstNodeID = 0;

    flo_html_TagRegistration initTag;
    initTag.tagID = 0;
    initTag.isPaired = false;
    initTag.hashElement.hash = 0;
    initTag.hashElement.offset = 0;

    flo_html_Registration initRegistration;
    initRegistration.flo_html_indexID = 0;
    initRegistration.hashElement.hash = 0;
    initRegistration.hashElement.offset = 0;

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
    dom->nodeLen = 1; // We start at 1 because 0 is used as error id, and
                      // otherwise we have to do [nodeID - 1] every time.
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
        destroyflo_html_Dom(dom);
        return DOM_ERROR_MEMORY;
    }

    flo_html_DomStatus domumentStatus =
        flo_html_parse(htmlString, dom, textStore);
    if (domumentStatus != DOM_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Failed to parse domument.\n");
    }
    return domumentStatus;
}

void destroyflo_html_Dom(flo_html_Dom *dom) {
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
