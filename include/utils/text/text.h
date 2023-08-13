#ifndef UTILS_TEXT_TEXT_H
#define UTILS_TEXT_TEXT_H

static inline unsigned char isAlphaBetical(const char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

static inline unsigned char isSpecialSpace(char ch) {
    return ch == '\t' || ch == '\n' || ch == '\r';
}

#endif
