#ifndef FLO_UTIL_HASH_HASHES_H
#define FLO_UTIL_HASH_HASHES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "flo/util/text/string.h"

__attribute__((unused)) static uint64_t
flo_hashStringSkeeto(flo_String string) {
    uint64_t h = 0x100;
    for (ptrdiff_t i = 0; i < string.len; i++) {
        h ^= string.buf[i];
        h *= 1111111111111111111u;
    }
    return h;
}

// http://www.cse.yorku.ca/~oz/hash.html
// djb2 hash
__attribute__((unused)) static uint64_t flo_hashStringDjb2(flo_String string) {
    uint64_t hash = 5381;
    for (ptrdiff_t i = 0; i < string.len; i++) {
        int c = (string.buf[i]);
        hash =
            ((hash << 5) + hash) ^
            c; // hash << 5 + hash because that is way cooler than hash * 33 :D
    }

    return hash;
}

// https://github.com/skeeto/hash-prospector
// 3-round xorshift-multiply (-Xn3)
// bias = 0.0045976709018820602
__attribute__((unused)) static uint16_t flo_hash16_xm3(uint16_t x) {
    x ^= x >> 7;
    x *= 0x2993U;
    x ^= x >> 5;
    x *= 0xe877U;
    x ^= x >> 9;
    x *= 0x0235U;
    x ^= x >> 10;
    return x;
}

#ifdef __cplusplus
}
#endif

#endif
