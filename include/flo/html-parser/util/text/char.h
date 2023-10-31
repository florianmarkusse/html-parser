#ifndef FLO_HTML_PARSER_UTIL_TEXT_CHAR_H
#define FLO_HTML_PARSER_UTIL_TEXT_CHAR_H

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((unused)) static inline unsigned char
flo_html_isAlphaBetical(unsigned char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

__attribute__((unused)) static inline unsigned char
flo_html_isSpecialSpace(unsigned char ch) {
    return ch == '\t' || ch == '\n' || ch == '\r';
}

#ifdef __cplusplus
}
#endif

#endif
