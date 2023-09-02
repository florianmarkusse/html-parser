
#include "flo/html-parser/dom/registry.h"
#include "flo/html-parser/utils/memory/memory.h"

DomStatus addTagRegistration(const indexID tagID, const bool isPaired,
                             const HashElement *hashElement, Dom *dom) {
    if ((dom->tagRegistry = resizeArray(
             dom->tagRegistry, dom->tagRegistryLen, &dom->tagRegistryCap,
             sizeof(TagRegistration), TAG_REGISTRATIONS_PER_PAGE)) == NULL) {
        return DOM_ERROR_MEMORY;
    }

    TagRegistration *tagRegistry = &(dom->tagRegistry[dom->tagRegistryLen]);
    tagRegistry->tagID = tagID;
    tagRegistry->isPaired = isPaired;
    tagRegistry->hashElement.hash = hashElement->hash;
    tagRegistry->hashElement.offset = hashElement->offset;
    dom->tagRegistryLen++;
    return DOM_SUCCESS;
}

DomStatus addRegistration(const indexID indexID, const HashElement *hashElement,
                          BasicRegistry *basicRegistry,
                          const size_t elementsPerPage) {
    if ((basicRegistry->registry = resizeArray(
             basicRegistry->registry, basicRegistry->len, &basicRegistry->cap,
             sizeof(Registration), elementsPerPage)) == NULL) {
        return DOM_ERROR_MEMORY;
    }

    Registration *registration = &(basicRegistry->registry[basicRegistry->len]);
    registration->indexID = indexID;
    registration->hashElement.hash = hashElement->hash;
    registration->hashElement.offset = hashElement->offset;
    basicRegistry->len++;
    return DOM_SUCCESS;
}

DomStatus addBoolPropRegistration(const indexID boolPropID,
                                  const HashElement *hashElement, Dom *dom) {
    return addRegistration(boolPropID, hashElement, &dom->boolPropRegistry,
                           BOOL_PROP_REGISTRATIONS_PER_PAGE);
}

DomStatus addPropKeyRegistration(indexID propKeyID,
                                 const HashElement *hashElement, Dom *dom) {
    return addRegistration(propKeyID, hashElement, &dom->propKeyRegistry,
                           PROP_KEY_REGISTRATIONS_PER_PAGE);
}

DomStatus addPropValueRegistration(indexID propValueID,
                                   const HashElement *hashElement, Dom *dom) {
    return addRegistration(propValueID, hashElement, &dom->propValueRegistry,
                           PROP_VALUE_REGISTRATIONS_PER_PAGE);
}
