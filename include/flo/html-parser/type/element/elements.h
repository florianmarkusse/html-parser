#ifndef FLO_HTML_PARSER_TYPE_ELEMENT_ELEMENTS_H
#define FLO_HTML_PARSER_TYPE_ELEMENT_ELEMENTS_H

#include <stddef.h>
#include <stdint.h>

#include "element-status.h"
#include "elements-container.h"
#include "flo/html-parser/hash/string-hash.h"

typedef struct {
    ElementsContainer container;
    StringHashSet set;
} __attribute__((aligned(64))) StringRegistry;

ElementStatus initStringRegistry(StringRegistry *stringRegistry,
                                 size_t stringsCapacity, size_t pageSize);
void destroyStringRegistry(StringRegistry *stringRegistry);

typedef struct {
    StringRegistry tags;
    StringRegistry boolProps;
    StringRegistry propKeys;
    StringRegistry propValues;
    ElementsContainer text;
} __attribute__((aligned(128))) TextStore;

ElementStatus createTextStore(TextStore *textStore);
void destroyTextStore(TextStore *textStore);

ElementStatus createElement(ElementsContainer *container, const char *element,
                            element_id *currentElementsLen, element_id offset,
                            element_id *elementID);

ElementStatus insertElement(ElementsContainer *elementsContainer,
                            const char *elementStart, size_t elementLength,
                            char **dataLocation);

ElementStatus elementToIndex(StringRegistry *stringRegistry,
                             const char *elementStart, size_t elementLength,
                             HashElement *hashElement, indexID *indexID);

#endif
