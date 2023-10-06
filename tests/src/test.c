#include <stdio.h>

#include "pretty-print.h"
#include "test.h"

static size_t tabLevel = 0;
inline void printTabs() {
    for (size_t i = 0; i < tabLevel; i++) {
        printf("\t");
    }
}

inline void printTestScore(const size_t successes, const size_t failures) {
    tabLevel = tabLevel == 0 ? 0 : tabLevel - 1;
    printTabs();
    printf("[ %zu / %lu ]\n", successes, failures + successes);
}

inline void printTestTopicStart(const char *testTopic) {
    printTabs();
    printf("Testing %s...\n", testTopic);
    tabLevel++;
}

inline void printTestStart(const char *testName) {
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

inline void printTestResultDifferenceErrorCode(const size_t expected,
                                               const char *expectedString,
                                               const size_t actual,
                                               const char *actualString) {
    printf("%-10s: %-4zu - %s\n", "Expected", expected, expectedString);
    printf("%-10s: %-4zu - %s\n", "Actual", actual, actualString);
}

inline void
printTestResultDifferenceString(const flo_html_String expectedString,
                                const flo_html_String actualString) {
    printf("%-20s: %.*s\n", "Expected string", FLO_HTML_S_P(expectedString));
    printf("%-20s: %.*s\n", "Actual string", FLO_HTML_S_P(actualString));
}

inline void printTestResultDifferenceNumber(const size_t expectedNumber,
                                            const size_t actualNumber) {
    printf("%-20s: %zu\n", "Expected number", expectedNumber);
    printf("%-20s: %zu\n", "Actual number", actualNumber);
}

inline void printTestResultDifferenceBool(const bool expectedBool,
                                          const bool actualBool) {
    printf("%-20s: %d\n", "Expected bool", expectedBool);
    printf("%-20s: %d\n", "Actual bool", actualBool);
}
