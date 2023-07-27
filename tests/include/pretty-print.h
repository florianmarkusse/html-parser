#ifndef PRETTY_PRINT_H
#define PRETTY_PRINT_H

#include <stdio.h>
#include <unistd.h>

// Function to check if ANSI escape codes are supported
static inline unsigned char supportsAnsiEscapeCodes() {
#ifdef _WIN32
    // Check for Windows console support
    DWORD consoleMode;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    return GetConsoleMode(hOut, &consoleMode) != 0;
#else
    // Check for Unix-like terminal support
    return isatty(fileno(stdout));
#endif
}

// ANSI color codes for text (fallback to empty strings if escape codes are not
// supported)
#define ANSI_COLOR_RED (supportsAnsiEscapeCodes() ? "\x1b[31m" : "")
#define ANSI_COLOR_GREEN (supportsAnsiEscapeCodes() ? "\x1b[32m" : "")
#define ANSI_COLOR_YELLOW (supportsAnsiEscapeCodes() ? "\x1b[33m" : "")
#define ANSI_COLOR_BLUE (supportsAnsiEscapeCodes() ? "\x1b[34m" : "")
#define ANSI_COLOR_MAGENTA (supportsAnsiEscapeCodes() ? "\x1b[35m" : "")
#define ANSI_COLOR_CYAN (supportsAnsiEscapeCodes() ? "\x1b[36m" : "")
#define ANSI_COLOR_RESET (supportsAnsiEscapeCodes() ? "\x1b[0m" : "")

#endif
