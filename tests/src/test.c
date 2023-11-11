#include <stdio.h>

#include "pretty-print.h"
#include "test.h"

static ptrdiff_t tabLevel = 0;
inline void printTabs() {
    for (ptrdiff_t i = 0; i < tabLevel; i++) {
        printf("\t");
    }
}

inline void printTestScore(ptrdiff_t successes,
                           ptrdiff_t failures) {
    tabLevel = tabLevel == 0 ? 0 : tabLevel - 1;
    printTabs();
    printf("[ %zu / %lu ]\n", successes, failures + successes);
}

inline void printTestTopicStart(char *testTopic) {
    printTabs();
    printf("Testing %s...\n", testTopic);
    tabLevel++;
}

inline void printTestStart(char *testName) {
    printTabs();
    printf("%-50s", testName);
}

inline void printTestSuccess() {
    printf("%s%-20s%s\n", ANSI_COLOR_GREEN, "Success", ANSI_COLOR_RESET);
}

inline void printTestFailure() {
    printf("%s%-20s%s\n", ANSI_COLOR_RED, "Failure", ANSI_COLOR_RESET);
}

inline void printTestDemarcation() {
    printf("---------------------------------------------------------------"
           "---\n");
}

inline void printTestResultDifferenceErrorCode(ptrdiff_t expected,
                                               char *expectedString,
                                               ptrdiff_t actual,
                                               char *actualString) {
    printf("%-10s: %-4zu - %s\n", "Expected", expected, expectedString);
    printf("%-10s: %-4zu - %s\n", "Actual", actual, actualString);
}

inline void
printTestResultDifferenceString(flo_String expectedString,
                                flo_String actualString) {
    printf("%-20s: %.*s\n", "Expected string", FLO_STRING_PRINT(expectedString));
    printf("%-20s: %.*s\n", "Actual string", FLO_STRING_PRINT(actualString));
}

inline void printTestResultDifferenceNumber(ptrdiff_t expectedNumber,
                                            ptrdiff_t actualNumber) {
    printf("%-20s: %zu\n", "Expected number", expectedNumber);
    printf("%-20s: %zu\n", "Actual number", actualNumber);
}

inline void printTestResultDifferenceBool(bool expectedBool,
                                          bool actualBool) {
    printf("%-20s: %d\n", "Expected bool", expectedBool);
    printf("%-20s: %d\n", "Actual bool", actualBool);
}
