#ifndef TYPE_NODE_TAG_H
#define TYPE_NODE_TAG_H

#include <stddef.h>

#define PAGE_SIZE (1U << 3U)
#define TOTAL_PAGES (1U << 4U)
#define TOTAL_TAGS (1U << 10U)
#define TOTAL_TAGS_MSB (TOTAL_TAGS >> 1U)
#define UNKNOWN_TAG (TOTAL_TAGS - 1)

typedef struct {
    char *start;
    char *freeSpace;
    size_t spaceLeft;
} __attribute__((aligned(32))) TagPage;

typedef struct {
    char *tags[TOTAL_TAGS];
    size_t pairedTagsLen;
    size_t singleTagsLen;
    TagPage pages[TOTAL_PAGES];
    size_t pageLen;
} __attribute__((aligned(128))) Tags;

Tags createTags();
void destroyTags(Tags *tags);

size_t tagToIndex(Tags *tags, const char *tagName, char isPaired);

#endif
