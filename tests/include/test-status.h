#ifndef TEST_STATUS_H
#define TEST_STATUS_H

typedef enum {
    TEST_SUCCESS,
    TEST_ERROR_INITIALIZATION,
    TEST_FAILURE,
    TEST_NUM_STATUS
} TestStatus;

static const char *const TestStatusStrings[TEST_NUM_STATUS] = {
    "Success", "Initialization Error", "Failure"};

static const char *testStatusToString(TestStatus status) {
    if (status >= 0 && status < TEST_NUM_STATUS) {
        return TestStatusStrings[status];
    }
    return "Unknown test status code!";
}

#endif
