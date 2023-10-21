#ifndef FLO_HTML_PARSER_UTIL_RAW_DATA_H
#define FLO_HTML_PARSER_UTIL_RAW_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "array.h"

typedef FLO_HTML_DYNAMIC_ARRAY(unsigned char) flo_html_RawData;

__attribute((unused)) static void flo_html_addRawData(flo_html_RawData *raw,
                                                      unsigned char *data,
                                                      ptrdiff_t bytes,
                                                      flo_html_Arena *perm) {
    if (raw->buf == NULL) {
        raw->cap = bytes;
        raw->buf = flo_html_alloc(perm, 2 * FLO_HTML_SIZEOF(unsigned char),
                                  FLO_HTML_ALIGNOF(unsigned char), raw->cap, 0);
    } else if (perm->end == (raw->buf - raw->cap)) {
        flo_html_alloc(perm, FLO_HTML_SIZEOF(unsigned char), 1, raw->cap, 0);
    } else {
        void *buf =
            flo_html_alloc(perm, 2 * FLO_HTML_SIZEOF(unsigned char),
                           FLO_HTML_ALIGNOF(unsigned char), raw->cap, 0);
        memcpy(buf, raw->buf, raw->len);
        raw->buf = buf;
    }

    raw->cap *= 2;
    memcpy(raw->buf + raw->len, data, bytes);
    raw->len += bytes;
}

#ifdef __cplusplus
}
#endif

#endif
