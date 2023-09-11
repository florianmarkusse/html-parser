#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/parser/parser.h"
#include "flo/html-parser/utils/file/read.h"
#include "flo/html-parser/utils/memory/memory.h"

void initBasicRegistry(BasicRegistry *basicRegistry,
                       const Registration *initRegistration) {
    basicRegistry->registry = malloc(PROP_REGISTRY_PAGE_SIZE);
    basicRegistry->registry[0] = *initRegistration;
    basicRegistry->len =
        1; // Start at 1 so we don't need to do tagRegistry[x - 1]
    basicRegistry->cap = PROP_REGISTRATIONS_PER_PAGE;
}

DomStatus createDomFromFile(const char *fileLocation, Dom *dom,
                            DataContainer *dataContainer) {
    char *buffer = NULL;
    FileStatus fileStatus = readFile(fileLocation, &buffer);
    if (fileStatus != FILE_SUCCESS) {
        ERROR_WITH_CODE_FORMAT(fileStatusToString(fileStatus),
                               "Failed to read file: \"%s\"", fileLocation);
        return DOM_ERROR_MEMORY;
    }

    DomStatus documentStatus = createDom(buffer, dom, dataContainer);
    if (documentStatus != DOM_SUCCESS) {
        FREE_TO_NULL(buffer);
        ERROR_WITH_CODE_FORMAT(documentStatusToString(documentStatus),
                               "Failed to create document from file \"%s\"",
                               fileLocation);
        return documentStatus;
    }
    FREE_TO_NULL(buffer);

    return DOM_SUCCESS;
}

DomStatus createDom(const char *htmlString, Dom *dom,
                    DataContainer *dataContainer) {
    dom->firstNodeID = 0;

    TagRegistration initTag;
    initTag.tagID = 0;
    initTag.isPaired = false;
    initTag.hashElement.hash = 0;
    initTag.hashElement.offset = 0;

    Registration initRegistration;
    initRegistration.indexID = 0;
    initRegistration.hashElement.hash = 0;
    initRegistration.hashElement.offset = 0;

    dom->tagRegistry = malloc(TAG_REGISTRY_PAGE_SIZE);
    dom->tagRegistry[0] = initTag;
    dom->tagRegistryLen =
        1; // Start at 1 so we don't need to do tagRegistry[x - 1]
    dom->tagRegistryCap = TAG_REGISTRATIONS_PER_PAGE;

    initBasicRegistry(&dom->boolPropRegistry, &initRegistration);
    initBasicRegistry(&dom->propKeyRegistry, &initRegistration);
    initBasicRegistry(&dom->propValueRegistry, &initRegistration);

    dom->nodes = malloc(NODES_PAGE_SIZE);
    Node errorNode;
    errorNode.nodeID = 0;
    errorNode.nodeType = NODE_TYPE_ERROR;
    errorNode.tagID = 0;
    dom->nodes[0] = errorNode;
    dom->nodeLen = 1; // We start at 1 because 0 is used as error id, and
                      // otherwise we have to do [nodeID - 1] every time.
    dom->nodeCap = NODES_PER_PAGE;

    dom->parentFirstChilds = malloc(PARENT_FIRST_CHILDS_PAGE_SIZE);
    dom->parentFirstChildLen = 0;
    dom->parentFirstChildCap = PARENT_FIRST_CHILDS_PER_PAGE;

    dom->parentChilds = malloc(PARENT_CHILDS_PAGE_SIZE);
    dom->parentChildLen = 0;
    dom->parentChildCap = PARENT_CHILDS_PER_PAGE;

    dom->nextNodes = malloc(NEXT_NODES_PAGE_SIZE);
    dom->nextNodeLen = 0;
    dom->nextNodeCap = NEXT_NODES_PER_PAGE;

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
        PRINT_ERROR("Failed to allocate memory for nodes.\n");
        destroyDom(dom);
        return DOM_ERROR_MEMORY;
    }

    DomStatus domumentStatus = parse(htmlString, dom, dataContainer);
    if (domumentStatus != DOM_SUCCESS) {
        PRINT_ERROR("Failed to parse domument.\n");
    }
    return domumentStatus;
}

void destroyDom(Dom *dom) {
    FREE_TO_NULL(dom->nodes);
    FREE_TO_NULL(dom->tagRegistry);
    FREE_TO_NULL(dom->boolPropRegistry.registry);
    FREE_TO_NULL(dom->propKeyRegistry.registry);
    FREE_TO_NULL(dom->propValueRegistry.registry);
    FREE_TO_NULL(dom->parentFirstChilds);
    FREE_TO_NULL(dom->parentChilds);
    FREE_TO_NULL(dom->nextNodes);
    FREE_TO_NULL(dom->boolProps);
    FREE_TO_NULL(dom->props);
}
