#ifndef TEST_H
#define TEST_H

#include <flo/html-parser/utils/text/string.h>
#include <stdbool.h>
#include <stddef.h>

void printTabs();
void printTestScore(ptrdiff_t successes, ptrdiff_t failures);
void printTestTopicStart(const char *testTopic);
void printTestStart(const char *testName);
void printTestSuccess();
void printTestFailure();
void printTestDemarcation();
void printTestResultDifferenceErrorCode(ptrdiff_t expected,
                                        const char *expectedString,
                                        ptrdiff_t actual,
                                        const char *actualString);
void printTestResultDifferenceString(const flo_html_String expectedString,
                                     const flo_html_String actualString);
void printTestResultDifferenceBool(bool expectedBool, bool actualBool);
void printTestResultDifferenceNumber(ptrdiff_t expectedNumber,
                                     ptrdiff_t actualNumber);

#endif
