
#include <stdbool.h>
#include <string.h>

#include "flo/html-parser/dom/modification/modification.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/dom/utils.h"
#include "flo/html-parser/type/node/tag-registration.h"
#include "flo/html-parser/utils/print/error.h"

const char *getTag(const indexID tagID, const Dom *dom,
                   const DataContainer *dataContainer) {
    TagRegistration *tagRegistration = &dom->tagRegistry[tagID];
    return getStringFromHashSet(&dataContainer->tags.set,
                                &tagRegistration->hashElement);
}

void getTagRegistration(indexID tagID, const Dom *dom,
                        TagRegistration **tagRegistration) {
    *tagRegistration = &dom->tagRegistry[tagID];
}

const char *getBoolProp(const indexID boolPropID, const Dom *dom,
                        const DataContainer *dataContainer) {
    Registration registration = dom->boolPropRegistry.registry[boolPropID];
    return getStringFromHashSet(&dataContainer->boolProps.set,
                                &registration.hashElement);
}

const char *getPropKey(const indexID propKeyID, const Dom *dom,
                       const DataContainer *dataContainer) {
    Registration registration = dom->propKeyRegistry.registry[propKeyID];
    return getStringFromHashSet(&dataContainer->propKeys.set,
                                &registration.hashElement);
}

const char *getPropValue(const indexID propValueID, const Dom *dom,
                         const DataContainer *dataContainer) {
    Registration registration = dom->propValueRegistry.registry[propValueID];
    return getStringFromHashSet(&dataContainer->propValues.set,
                                &registration.hashElement);
}

MergeResult tryMerge(Node *possibleMergeNode, Node *replacingNode, Dom *dom,
                     DataContainer *dataContainer, bool isAppend) {
    if (possibleMergeNode->nodeType == NODE_TYPE_TEXT) {
        ElementStatus elementStatus = addTextToTextNode(
            possibleMergeNode, replacingNode->text, strlen(replacingNode->text),
            dom, dataContainer, isAppend);
        if (elementStatus != ELEMENT_CREATED) {
            PRINT_ERROR("Failed to merge new text node with up node!\n");
            return FAILED_MERGE;
        }
        return COMPLETED_MERGE;
    }
    return NO_MERGE;
}

DomStatus connectOtherNodesToParent(const node_id parentID,
                                    const node_id lastAddedChild, Dom *dom) {
    node_id otherNewNodeID = getNext(lastAddedChild, dom);
    while (otherNewNodeID > 0) {
        DomStatus domStatus = addParentChild(parentID, otherNewNodeID, dom);
        if (domStatus != DOM_SUCCESS) {
            PRINT_ERROR("Failed to add new node ID as child!\n");
            return domStatus;
        }
        otherNewNodeID = getNext(otherNewNodeID, dom);
    }

    return DOM_SUCCESS;
}
