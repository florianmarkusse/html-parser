#ifndef FLO_HTML_PARSER_UTIL_STRING_AUTO_UINT16_MAP_H
#define FLO_HTML_PARSER_UTIL_STRING_AUTO_UINT16_MAP_H

#include "flo/util/hash/trie/common-iterator.h"
#include "flo/util/text/string.h"

typedef struct {
    flo_String key;
    uint16_t value;
} flo_trie_StringAutoUint16Data;

typedef struct flo_trie_StringAutoUint16Node flo_trie_StringAutoUint16Node;
struct flo_trie_StringAutoUint16Node {
    struct flo_trie_StringAutoUint16Node *child[4];
    flo_trie_StringAutoUint16Data data;
};

typedef struct {
    uint16_t identity;
    flo_trie_StringAutoUint16Node *node;
} flo_trie_StringAutoUint16Map;

typedef struct {
    bool wasInserted;
    uint16_t entryIndex;
} flo_NewStringInsert;

flo_NewStringInsert flo_trie_insertStringAutoUint16Map(
    flo_String key, flo_trie_StringAutoUint16Map *set, flo_Arena *perm);

uint16_t
flo_trie_containsStringAutoUint16Map(flo_String key,
                                     flo_trie_StringAutoUint16Map *set);

FLO_TRIE_ITERATOR_HEADER_FILE(flo_trie_StringAutoUint16Node,
                              flo_trie_StringAutoUint16IterNode,
                              flo_trie_StringAutoUint16Iterator,
                              flo_trie_StringAutoUint16Data,
                              flo_createStringAutoUint16Iterator,
                              flo_nextStringAutoUint16Iterator);

#define FLO_FOR_EACH_TRIE_STRING_AUTO_UINT16(element, stringAutoUint16Map,     \
                                             scratch)                          \
    for (flo_trie_StringAutoUint16Iterator *iter =                             \
             flo_createStringAutoUint16Iterator(stringAutoUint16Map,           \
                                                &(scratch));                   \
         ;)                                                                    \
        if (((element) = flo_nextStringAutoUint16Iterator(iter, &(scratch)))   \
                .value == 0)                                                   \
            break;                                                             \
        else

#endif
