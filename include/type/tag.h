#ifndef TYPE_TAG_H
#define TYPE_TAG_H

#include <stddef.h>
#include <stdint.h>

#include "data-page.h"
#include "tag-status.h"

#define TOTAL_TAGS (1U << 10U)
#define TAGS_TOTAL_PAGES (1U << 4U)
typedef uint16_t tag_id;

#define TOTAL_TAGS_NUM_BITS (sizeof(tag_id) * 8)
// The MSB is used as a mask to indicate single tags.
#define TOTAL_TAGS_MSB (TOTAL_TAGS >> 1U)

typedef struct {
    char *tags[TOTAL_TAGS];
    tag_id pairedTagsLen;
    tag_id singleTagsLen;
    DataPage pages[TAGS_TOTAL_PAGES];
    page_id pageLen;
} __attribute__((aligned(128))) Tags;

extern Tags globalTags;

TagStatus createTags();
void destroyTags();

TagStatus tagToIndex(const char *tagStart, size_t tagLength,
                     unsigned char isPaired, tag_id *tagID);
unsigned char isSelfClosing(tag_id index);

void printTagStatus();

#endif
