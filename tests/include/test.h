#ifndef TEST_H
#define TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <text/string.h>
#include <stdbool.h>
#include <stddef.h>

void printTabs();
void printTestScore(ptrdiff_t successes, ptrdiff_t failures);
void printTestTopicStart(char *testTopic);
void printTestStart(char *testName);
void printTestSuccess();
void printTestFailure();
void printTestDemarcation();
void printTestResultDifferenceErrorCode(ptrdiff_t expected,
                                        char *expectedString,
                                        ptrdiff_t actual,
                                        char *actualString);
void printTestResultDifferenceString(flo_String expectedString,
                                     flo_String actualString);
void printTestResultDifferenceBool(bool expectedBool, bool actualBool);
void printTestResultDifferenceNumber(ptrdiff_t expectedNumber,
                                     ptrdiff_t actualNumber);

#ifdef __cplusplus
}
#endif

#endif
