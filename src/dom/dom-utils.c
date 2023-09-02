
#include "flo/html-parser/dom/utils.h"
#include "flo/html-parser/type/node/tag-registration.h"

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
