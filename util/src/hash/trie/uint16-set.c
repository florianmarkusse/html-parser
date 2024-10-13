#include "flo/util/hash/trie/uint16-set.h"

bool flo_trie_insertUint16Set(uint16_t key, flo_trie_Uint16Set **set,
                              flo_Arena *perm) {
    FLO_ASSERT(key != 0);
    for (uint16_t hash = flo_hash16_xm3(key); *set != NULL; hash <<= 2) {
        if (key == (*set)->data) {
            return false;
        }
        set = &(*set)->child[hash >> 14];
    }
    *set = FLO_NEW(perm, flo_trie_Uint16Set, 1, FLO_ZERO_MEMORY);
    (*set)->data = key;
    return true;
}

FLO_TRIE_ITERATOR_SOURCE_FILE(flo_trie_Uint16Set, flo_trie_Uint16IterNode,
                              flo_trie_Uint16Iterator, uint16_t,
                              flo_createUint16Iterator, flo_nextUint16Iterator);
