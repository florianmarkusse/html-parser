#ifndef TYPE_ELEMENT_ELEMENTS_H
#define TYPE_ELEMENT_ELEMENTS_H

#include <stddef.h>
#include <stdint.h>

#include "element-status.h"
#include "elements-container.h"

// The MSB is used as a mask to indicate singles.
#define TOTAL_ELEMENTS_MSB (EXPONENT - 1)
#define SINGLE_OFFSET (1U << (EXPONENT - 1))

typedef uint16_t element_id;
#define TOTAL_ELEMENTS_NUM_BITS (sizeof(element_id) * 8)

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

ElementStatus combinedElementToIndex(CombinedElements *global,
                                     const char *elementStart,
                                     size_t elementLength,
                                     unsigned char isPaired,
                                     element_id *elementID);
ElementStatus elementToIndex(Elements *global, const char *elementStart,
                             size_t elementLength, element_id *elementID);
ElementStatus createElement(ElementsContainer *container,
                            const char *elementName,
                            element_id *currentElementLen, element_id offset,
                            element_id *elementID);

unsigned char isSingle(element_id index);

void printGlobalTagStatus();
void printGlobalAttributeStatus();

#endif
