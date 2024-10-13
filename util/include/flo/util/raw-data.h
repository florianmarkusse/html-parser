#ifndef FLO_UTIL_RAW_DATA_H
#define FLO_UTIL_RAW_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "flo/util/array.h"
#include "flo/util/text/string.h"

typedef FLO_DYNAMIC_ARRAY(unsigned char) flo_RawData;

// TODO: create some kind of batch push macro that fulfills this need?
__attribute((unused)) static void
flo_addRawData(flo_RawData *raw, flo_String data, flo_Arena *perm) {
    if (raw->len + data.len > raw->cap) {
        ptrdiff_t newCap = (raw->len + data.len) * 2;
        if (raw->buf == NULL) {
            raw->cap = data.len;
            raw->buf = flo_alloc(perm, FLO_SIZEOF(unsigned char),
                                 FLO_ALIGNOF(unsigned char), newCap, 0);
        } else if (perm->end == (char *)(raw->buf - raw->cap)) {
            flo_alloc(perm, FLO_SIZEOF(unsigned char),
                      FLO_ALIGNOF(unsigned char), newCap, 0);
        } else {
            void *buf = flo_alloc(perm, FLO_SIZEOF(unsigned char),
                                  FLO_ALIGNOF(unsigned char), newCap, 0);
            memcpy(buf, raw->buf, raw->len);
            raw->buf = buf;
        }

        raw->cap = newCap;
    }
    memcpy(raw->buf + raw->len, data.buf, data.len);
    raw->len += data.len;
}

#ifdef __cplusplus
}
#endif

#endif
