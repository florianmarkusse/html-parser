#ifndef FLO_HTML_PARSER_UTIL_STRING_AUTO_UINT16_MAP_H
#define FLO_HTML_PARSER_UTIL_STRING_AUTO_UINT16_MAP_H

#include "text/string.h"

typedef struct flo_trie_StringAutoUint16Map flo_trie_StringAutoUint16Map;
struct flo_trie_StringAutoUint16Map {
    struct flo_trie_StringAutoUint16Map *child[4];
    flo_String key;
    uint16_t value;
};

typedef struct {
    bool wasInserted;
    uint16_t entryIndex;
} flo_NewStringInsert;

flo_NewStringInsert flo_trie_insertStringAutoUint16Map(
    flo_String key, flo_trie_StringAutoUint16Map **set, flo_Arena *perm);

uint16_t
flo_trie_containsStringAutoUint16Map(flo_String key,
                                     flo_trie_StringAutoUint16Map **set);

#endif
