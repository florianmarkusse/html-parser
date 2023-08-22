#ifndef FLO_HTML_PARSER_HASH_HASHES_H
#define FLO_HTML_PARSER_HASH_HASHES_H

#include "flo/html-parser/type/element/elements-container.h"

// http://www.cse.yorku.ca/~oz/hash.html
// djb2 hash
static size_t hashString(const char *string) {
    size_t hash = 5381;
    int c = 0;

    while (c = *string++) {
        hash =
            ((hash << 5) + hash) ^
            c; // hash << 5 + hash because that is way cooler than hash * 33 :D
    }

    return hash;
}

#ifdef ELEMENT_ID_UINT_16

// https://github.com/skeeto/hash-prospector
// 3-round xorshift-multiply (-Xn3)
// bias = 0.0045976709018820602
static uint16_t hash16_xm3(uint16_t x) {
    x ^= x >> 7;
    x *= 0x2993U;
    x ^= x >> 5;
    x *= 0xe877U;
    x ^= x >> 9;
    x *= 0x0235U;
    x ^= x >> 10;
    return x;
}
#endif

#endif
