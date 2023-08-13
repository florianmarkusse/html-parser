#ifndef TEST_H
#define TEST_H

#include <stddef.h>
#include <stdio.h>

#include "pretty-print.h"

static inline void printTestScore(const size_t successes,
                                  const size_t failures) {
    printf("[ %zu / %lu ]\n", successes, failures + successes);
}

static inline void printTestTopicStart(const char *testTopic) {
    printf("Testing %s...\n", testTopic);
}

static inline void printTestStart(const char *testName) {
    printf("    Testing %-50s", testName);
}

static inline void printTestSuccess() {
    printf("%s%-20s%s\n", ANSI_COLOR_GREEN, "Success", ANSI_COLOR_RESET);
}

static inline void printTestFailure() {
    printf("%s%-20s%s\n", ANSI_COLOR_RED, "Failure", ANSI_COLOR_RESET);
}

static inline void printTestDemarcation() {
    printf("---------------------------------------------------------------"
           "---\n");
}

static inline void printTestResultDifferenceString(const size_t expected,
                                                   const char *expectedString,
                                                   const size_t actual,
                                                   const char *actualString) {
    printf("%-10s: %-4zu - %s\n", "Expected", expected, expectedString);
    printf("%-10s: %-4zu - %s\n", "Actual", actual, actualString);
}

static inline void printTestResultDifferenceNumber(const size_t expected,
                                                   const size_t expectedNumber,
                                                   const size_t actual,
                                                   const size_t actualNumber) {
    printf("%-20s: %-4zu - %zu\n", "Expected number", expected, expectedNumber);
    printf("%-20s: %-4zu - %zu\n", "Actual number", actual, actualNumber);
}

#endif
