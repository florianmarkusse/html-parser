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
} __attribute__((aligned(64))) NewElements;

typedef struct {
    ElementsContainer container;
    element_id pairedLen;
    element_id singleLen;
} __attribute__((aligned(64))) CombinedElements;

typedef struct {
    ElementsContainer container;
    element_id len;
} __attribute__((aligned(64))) Elements;

typedef struct {
    NewElements tagNames;
    CombinedElements tags;
    CombinedElements propKeys;
    Elements propValues;
    Elements text;
} __attribute__((aligned(128))) DataContainer;

ElementStatus createDataContainer(DataContainer *dataContainer);
void destroyDataContainer(DataContainer *dataContainer);

ElementStatus findElement(const ElementsContainer *container,
                          const element_id *currentElementLen,
                          const char *elementName, element_id offsetMask,
                          element_id *elementID);
ElementStatus elementToIndex(ElementsContainer *container,
                             element_id *currentElementLen,
                             const char *elementStart, size_t elementLength,
                             unsigned char isPaired,
                             unsigned char searchElements,
                             element_id *elementID);

ElementStatus textElementToIndex(element_id *elementID);
ElementStatus createElement(ElementsContainer *container, const char *element,
                            element_id *currentElementsLen, element_id offset,
                            element_id *elementID);

unsigned char isSingle(element_id index);
unsigned char isText(element_id index);

ElementStatus newElementToIndex(NewElements *newElements,
                                const char *elementStart, size_t elementLength,
                                bool searchElements, HashElement *hashElement,
                                indexID *indexID);

#endif
