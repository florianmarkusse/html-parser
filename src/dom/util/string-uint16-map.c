#include "flo/html-parser/util/string-auto-uint16-map.h"

#include "hash/hashes.h"

static uint16_t identity = 0;

flo_NewStringInsert flo_trie_insertStringAutoUint16Map(
    flo_String key, flo_trie_StringAutoUint16Map **set, flo_Arena *perm) {
    FLO_ASSERT(key.len > 0);
    for (uint64_t hash = flo_hashString(key); *set != NULL; hash <<= 2) {
        if (flo_stringEquals(key, (*set)->key)) {
            return (flo_NewStringInsert){.entryIndex = (*set)->value,
                                         .wasInserted = false};
        }
        set = &(*set)->child[hash >> 62];
    }
    *set = FLO_NEW(perm, flo_trie_StringAutoUint16Map, 1, FLO_ZERO_MEMORY);
    (*set)->key = key;
    (*set)->value = ++identity;
    return (flo_NewStringInsert){.wasInserted = true, .entryIndex = identity};
}

uint16_t
flo_trie_containsStringAutoUint16Map(flo_String key,
                                     flo_trie_StringAutoUint16Map **set) {
    for (uint64_t hash = flo_hashString(key); *set != NULL; hash <<= 2) {
        if (flo_stringEquals(key, (*set)->key)) {
            return (*set)->value;
        }
        set = &(*set)->child[hash >> 62];
    }
    return 0;
}
