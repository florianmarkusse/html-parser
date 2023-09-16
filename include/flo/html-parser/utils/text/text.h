#ifndef FLO_HTML_PARSER_UTILS_TEXT_TEXT_H
#define FLO_HTML_PARSER_UTILS_TEXT_TEXT_H

static inline unsigned char flo_html_isAlphaBetical(const char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

static inline unsigned char flo_html_isSpecialSpace(char ch) {
    return ch == '\t' || ch == '\n' || ch == '\r';
}

#endif
