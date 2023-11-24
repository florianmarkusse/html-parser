#ifndef TEST_H
#define TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <text/string.h>

void printTabs();
void printTestScore(ptrdiff_t successes, ptrdiff_t failures);
void printTestTopicStart(char *testTopic);
void printTestStart(char *testName);
void printTestSuccess();
void printTestFailure();
void printTestDemarcation();
void printTestResultDifferenceErrorCode(ptrdiff_t expected,
                                        flo_String expectedString,
                                        ptrdiff_t actual,
                                        flo_String actualString);
void printTestResultDifferenceString(flo_String expectedString,
                                     flo_String actualString);
void printTestResultDifferenceBool(bool expectedBool, bool actualBool);
void printTestResultDifferenceNumber(ptrdiff_t expectedNumber,
                                     ptrdiff_t actualNumber);

#ifdef __cplusplus
}
#endif

#endif
