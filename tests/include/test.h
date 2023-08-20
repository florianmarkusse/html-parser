#ifndef TEST_H
#define TEST_H

#include <stddef.h>

void printTabs();
void printTestScore(size_t successes, size_t failures);
void printTestTopicStart(const char *testTopic);
void printTestStart(const char *testName);
void printTestSuccess();
void printTestFailure();
void printTestDemarcation();
void printTestResultDifferenceString(size_t expected,
                                     const char *expectedString, size_t actual,
                                     const char *actualString);
void printTestResultDifferenceNumber(size_t expectedNumber,
                                     size_t actualNumber);
#endif
