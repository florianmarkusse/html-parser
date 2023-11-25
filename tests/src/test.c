#include <stdio.h>

#include "log.h"
#include "test.h"

static ptrdiff_t tabLevel = 0;
inline void printTabs() {
    for (ptrdiff_t i = 0; i < tabLevel; i++) {
        FLO_INFO("\t");
    }
}

inline void printTestScore(ptrdiff_t successes, ptrdiff_t failures) {
    FLO_FLUSH_AFTER(FLO_STDOUT) {
        tabLevel = tabLevel == 0 ? 0 : tabLevel - 1;
        printTabs();

        FLO_INFO((FLO_STRING("[ ")));
        FLO_INFO(successes);
        FLO_INFO((FLO_STRING(" / ")));
        FLO_INFO(failures + successes);
        FLO_INFO((FLO_STRING(" ]\n")));
    }
}

inline void printTestTopicStart(flo_String testTopic) {
    FLO_FLUSH_AFTER(FLO_STDOUT) {
        printTabs();
        FLO_INFO((FLO_STRING("Testing ")));
        FLO_INFO(testTopic);
        FLO_INFO((FLO_STRING("...\n")));
        tabLevel++;
    }
}

inline void printTestStart(flo_String testName) {
    FLO_FLUSH_AFTER(FLO_STDOUT) {
        printTabs();
        flo_appendToBufferMinSize(testName, 50, flo_getWriteBuffer(FLO_STDOUT),
                                  0);
    }
}

inline void printTestSuccess() {
    FLO_FLUSH_AFTER(FLO_STDOUT) {
        flo_appendColor(FLO_COLOR_GREEN, FLO_STDOUT);
        flo_appendToBufferMinSize(FLO_STRING("Success"), 20,
                                  flo_getWriteBuffer(FLO_STDOUT), 0);
        flo_appendColorReset(FLO_STDOUT);
        FLO_INFO((FLO_STRING("\n")));
    }
}

inline void printTestFailure() {
    flo_appendColor(FLO_COLOR_RED, FLO_STDERR);
    flo_appendToBufferMinSize(FLO_STRING("Failure"), 20,
                              flo_getWriteBuffer(FLO_STDERR), 0);
    flo_appendColorReset(FLO_STDERR);
    FLO_ERROR((FLO_STRING("\n")));
}

inline void printTestDemarcation() {
    FLO_ERROR((FLO_STRING(
        "---------------------------------------------------------------"
        "---\n")));
}

inline void printTestResultDifferenceErrorCode(ptrdiff_t expected,
                                               flo_String expectedString,
                                               ptrdiff_t actual,
                                               flo_String actualString) {
    flo_appendToBufferMinSize(FLO_STRING("Expected"), 10,
                              flo_getWriteBuffer(FLO_STDERR), 0);
    FLO_ERROR((FLO_STRING(": ")));
    flo_appendPtrDiffToBufferMinSize(expected, 4,
                                     flo_getWriteBuffer(FLO_STDERR), 0);
    FLO_ERROR((FLO_STRING(" - ")));
    FLO_ERROR(expectedString, FLO_NEWLINE);

    flo_appendToBufferMinSize(FLO_STRING("Actual"), 10,
                              flo_getWriteBuffer(FLO_STDERR), 0);
    FLO_ERROR((FLO_STRING(": ")));
    flo_appendPtrDiffToBufferMinSize(actual, 4, flo_getWriteBuffer(FLO_STDERR),
                                     0);
    FLO_ERROR((FLO_STRING(" - ")));
    FLO_ERROR(actualString, FLO_NEWLINE);
}

inline void printTestResultDifferenceString(flo_String expectedString,
                                            flo_String actualString) {
    flo_appendToBufferMinSize(FLO_STRING("Expected string"), 20,
                              flo_getWriteBuffer(FLO_STDERR), 0);
    FLO_ERROR((FLO_STRING(": ")));
    FLO_ERROR(expectedString, FLO_NEWLINE);

    flo_appendToBufferMinSize(FLO_STRING("Actual string"), 20,
                              flo_getWriteBuffer(FLO_STDERR), 0);
    FLO_ERROR((FLO_STRING(": ")));
    FLO_ERROR(actualString, FLO_NEWLINE);
}

inline void printTestResultDifferenceNumber(ptrdiff_t expectedNumber,
                                            ptrdiff_t actualNumber) {
    flo_appendToBufferMinSize(FLO_STRING("Expected number"), 20,
                              flo_getWriteBuffer(FLO_STDERR), 0);
    FLO_ERROR((FLO_STRING(": ")));
    FLO_ERROR(expectedNumber, FLO_NEWLINE);

    flo_appendToBufferMinSize(FLO_STRING("Actual number"), 20,
                              flo_getWriteBuffer(FLO_STDERR), 0);
    FLO_ERROR((FLO_STRING(": ")));
    FLO_ERROR(actualNumber, FLO_NEWLINE);
}

inline void printTestResultDifferenceBool(bool expectedBool, bool actualBool) {
    flo_appendToBufferMinSize(FLO_STRING("Expected bool"), 20,
                              flo_getWriteBuffer(FLO_STDERR), 0);
    FLO_ERROR((FLO_STRING(": ")));
    FLO_ERROR(expectedBool, FLO_NEWLINE);

    flo_appendToBufferMinSize(FLO_STRING("Actual bool"), 20,
                              flo_getWriteBuffer(FLO_STDERR), 0);
    FLO_ERROR((FLO_STRING(": ")));
    FLO_ERROR(actualBool, FLO_NEWLINE);
}
