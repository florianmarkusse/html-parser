#ifndef TEST_H
#define TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "log.h"
#include <stdbool.h>
#include <stddef.h>
#include <text/string.h>

#define FLO_LOG_TEST_FAILED                                                    \
    for (ptrdiff_t i = (printTestFailure(), printTestDemarcation(), 0); i < 1; \
         i = (printTestDemarcation(), FLO_FLUSH_TO(FLO_STDERR), 1))

void printTabs();
void printTestScore(ptrdiff_t successes, ptrdiff_t failures);
void printTestTopicStart(flo_String testTopic);
void printTestStart(flo_String testName);
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
