#ifndef TYPE_ELEMENT_H
#define TYPE_ELEMENT_H

#include <stddef.h>
#include <stdint.h>

#include "data-page.h"
#include "element-status.h"

#define EXPONENT 10U
#define TOTAL_ELEMENTS (1U << EXPONENT)
// The MSB is used as a mask to indicate singles.
#define TOTAL_ELEMENTS_MSB (EXPONENT - 1)
#define SINGLE_TAGS_OFFSET (1U << (EXPONENT - 1))

#define TOTAL_PAGES (1U << 4U)

typedef uint16_t element_id;
#define TOTAL_ELEMENTS_NUM_BITS (sizeof(element_id) * 8)

typedef struct {
    char *elements[TOTAL_ELEMENTS];
    element_id pairedLen;
    element_id singleLen;
    DataPage pages[TOTAL_PAGES];
    page_id pageLen;
} __attribute__((aligned(128))) Elements;

extern Elements globalTags;
extern Elements globalAttributes;

ElementStatus createGlobals();
void destroyGlobals();

ElementStatus elementToIndex(Elements *global, const char *elementStart,
                             size_t elementLength, unsigned char isPaired,
                             element_id *elementID);
unsigned char isSingle(element_id index);

void printElementStatus();

#endif
