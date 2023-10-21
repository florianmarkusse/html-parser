#ifndef FLO_HTML_PARSER_UTIL_RAW_DATA_H
#define FLO_HTML_PARSER_UTIL_RAW_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "array.h"
#include "text/string.h"

typedef FLO_HTML_DYNAMIC_ARRAY(unsigned char) flo_html_RawData;

// TODO: create some kind of batch push macro that fulfills this need?
__attribute((unused)) static void flo_html_addRawData(flo_html_RawData *raw,
                                                      flo_html_String data,
                                                      flo_html_Arena *perm) {
    if (raw->len + data.len > raw->cap) {
        ptrdiff_t newCap = (raw->len + data.len) * 2;
        if (raw->buf == NULL) {
            raw->cap = data.len;
            raw->buf =
                flo_html_alloc(perm, FLO_HTML_SIZEOF(unsigned char),
                               FLO_HTML_ALIGNOF(unsigned char), newCap, 0);
        } else if (perm->end == (char *)(raw->buf - raw->cap)) {
            flo_html_alloc(perm, FLO_HTML_SIZEOF(unsigned char),
                           FLO_HTML_ALIGNOF(unsigned char), newCap, 0);
        } else {
            void *buf =
                flo_html_alloc(perm, FLO_HTML_SIZEOF(unsigned char),
                               FLO_HTML_ALIGNOF(unsigned char), newCap, 0);
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
