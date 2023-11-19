#include "flo/html-parser/util/string-auto-uint16-map.h"

#include "hash/hashes.h"

static uint16_t identity = 0;

flo_NewStringInsert flo_trie_insertStringAutoUint16Map(
    flo_String key, flo_trie_StringAutoUint16Map **set, flo_Arena *perm) {
    for (uint64_t hash = flo_hashString(key); *set != NULL; hash <<= 2) {
        if (flo_stringEquals(key, (*set)->data.key)) {
            return (flo_NewStringInsert){.entryIndex = (*set)->data.value,
                                         .wasInserted = false};
        }
        set = &(*set)->child[hash >> 62];
    }
    if (identity == UINT16_MAX) {
        FLO_ASSERT(false);
        __builtin_longjmp(perm->jmp_buf, 1);
    }
    *set = FLO_NEW(perm, flo_trie_StringAutoUint16Map, 1, FLO_ZERO_MEMORY);
    (*set)->data.key = key;
    (*set)->data.value = ++identity;
    return (flo_NewStringInsert){.wasInserted = true, .entryIndex = identity};
}

uint16_t
flo_trie_containsStringAutoUint16Map(flo_String key,
                                     flo_trie_StringAutoUint16Map **set) {
    for (uint64_t hash = flo_hashString(key); *set != NULL; hash <<= 2) {
        if (flo_stringEquals(key, (*set)->data.key)) {
            return (*set)->data.value;
        }
        set = &(*set)->child[hash >> 62];
    }
    return 0;
}

FLO_TRIE_ITERATOR_SOURCE_FILE(flo_trie_StringAutoUint16Map,
                              flo_trie_StringAutoUint16IterNode,
                              flo_trie_StringAutoUint16Iterator,
                              flo_trie_StringAutoUint16Data,
                              flo_createStringAutoUint16Iterator,
                              flo_nextStringAutoUint16Iterator);
