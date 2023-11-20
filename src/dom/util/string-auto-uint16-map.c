#include "flo/html-parser/util/string-auto-uint16-map.h"

#include "hash/hashes.h"

flo_NewStringInsert flo_trie_insertStringAutoUint16Map(
    flo_String key, flo_trie_StringAutoUint16Map *set, flo_Arena *perm) {
    flo_trie_StringAutoUint16Node **currentNode = &set->node;
    for (uint64_t hash = flo_hashStringDjb2(key); *currentNode != NULL;
         hash <<= 2) {
        if (flo_stringEquals(key, (*currentNode)->data.key)) {
            return (flo_NewStringInsert){
                .entryIndex = (*currentNode)->data.value, .wasInserted = false};
        }
        currentNode = &(*currentNode)->child[hash << 62];
    }
    if (set->identity == UINT16_MAX) {
        FLO_ASSERT(false);
        __builtin_longjmp(perm->jmp_buf, 1);
    }
    *currentNode =
        FLO_NEW(perm, flo_trie_StringAutoUint16Node, 1, FLO_ZERO_MEMORY);
    (*currentNode)->data.key = key;
    (*currentNode)->data.value = ++(set)->identity;
    return (flo_NewStringInsert){.wasInserted = true,
                                 .entryIndex = (set)->identity};
}

uint16_t
flo_trie_containsStringAutoUint16Map(flo_String key,
                                     flo_trie_StringAutoUint16Map *set) {
    flo_trie_StringAutoUint16Node **currentNode = &set->node;
    for (uint64_t hash = flo_hashStringDjb2(key); *currentNode != NULL;
         hash <<= 2) {
        if (flo_stringEquals(key, (*currentNode)->data.key)) {
            return (*currentNode)->data.value;
        }
        currentNode = &(*currentNode)->child[hash << 62];
    }
    return 0;
}

FLO_TRIE_ITERATOR_SOURCE_FILE(flo_trie_StringAutoUint16Node,
                              flo_trie_StringAutoUint16IterNode,
                              flo_trie_StringAutoUint16Iterator,
                              flo_trie_StringAutoUint16Data,
                              flo_createStringAutoUint16Iterator,
                              flo_nextStringAutoUint16Iterator);
