#include "flo/util/hash/trie/string-set.h"

bool flo_trie_insertStringSet(flo_String key, flo_trie_StringSet **set,
                              flo_Arena *perm) {
    FLO_ASSERT(key.len > 0);
    for (uint64_t hash = flo_hashStringDjb2(key); *set != NULL; hash <<= 2) {
        if (flo_stringEquals(key, (*set)->data)) {
            return false;
        }
        set = &(*set)->child[hash >> 62];
    }
    *set = FLO_NEW(perm, flo_trie_StringSet, 1, FLO_ZERO_MEMORY);
    (*set)->data = key;
    return true;
}

FLO_TRIE_ITERATOR_SOURCE_FILE(flo_trie_StringSet, flo_trie_StringIterNode,
                              flo_trie_StringIterator, flo_String,
                              flo_createStringIterator, flo_nextStringIterator);
