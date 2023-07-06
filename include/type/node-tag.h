#ifndef TYPE_NODE_TAG_H
#define TYPE_NODE_TAG_H

#include <stddef.h>
#include <stdint.h>

#include "data-page.h"

#define TOTAL_TAGS (1U << 10U)
typedef uint16_t tag_id;
// Top x values are used as error codes.
#define ERROR_CODES_START (TOTAL_TAGS - (1U << 4U))

#define TOTAL_TAGS_NUM_BITS (sizeof(tag_id) * 8)
// The MSB is used as a mask to indicate single tags.
#define TOTAL_TAGS_MSB (TOTAL_TAGS >> 1U)

#define TAG_TOO_LONG (ERROR_CODES_START)
#define FAILED_TO_ALLOCATE_FOR_PAGE_OF_TAGS (ERROR_CODES_START + 1)
#define NO_CAPACITY (ERROR_CODES_START + 2)

typedef struct {
    char *tags[TOTAL_TAGS];
    tag_id pairedTagsLen;
    tag_id singleTagsLen;
    DataPage pages[TOTAL_PAGES];
    page_id pageLen;
} __attribute__((aligned(128))) Tags;

extern Tags globalTags;

tag_id createTags();
void destroyTags();

tag_id tagToIndex(const char *tagName, unsigned char isPaired);
unsigned char isSelfClosing(tag_id index);

void printTagStatus();

#endif
