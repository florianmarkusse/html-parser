#ifndef TYPE_ELEMENT_ELEMENTS_H
#define TYPE_ELEMENT_ELEMENTS_H

#include <stddef.h>
#include <stdint.h>

#include "element-status.h"
#include "elements-container.h"

typedef struct {
    ElementsContainer container;
    element_id pairedLen;
    element_id singleLen;
} __attribute__((aligned(128))) CombinedElements;

typedef struct {
    ElementsContainer container;
    element_id len;
} __attribute__((aligned(128))) Elements;

extern CombinedElements gTags;
extern CombinedElements gPropKeys;
extern Elements gPropValues;
extern Elements gText;

ElementStatus createGlobals();
void destroyGlobals();

ElementStatus elementToIndex(ElementsContainer *container,
                             element_id *currentElementLen,
                             const char *elementStart, size_t elementLength,
                             unsigned char isText, unsigned char isPaired,
                             element_id *elementID);

ElementStatus createElement(ElementsContainer *container, const char *element,
                            element_id *currentElementsLen, element_id offset,
                            element_id *elementID);

unsigned char isSingle(element_id index);
unsigned char isText(element_id index);

void printGlobalTagStatus();
void printGlobalAttributeStatus();
void printGlobalTextStatus();

#endif
