#ifndef TEST_STATUS_H
#define TEST_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    TEST_SUCCESS,
    TEST_ERROR_INITIALIZATION,
    TEST_FAILURE,
    TEST_NUM_STATUS
} TestStatus;

static char *TestStatusStrings[TEST_NUM_STATUS] = {
    "Success", "Initialization Error", "Failure"};

// Not always used, but very handy for those that actually do want readable
// error codes.
__attribute__((unused)) static char *
testStatusToString(TestStatus status) {
    if (status >= 0 && status < TEST_NUM_STATUS) {
        return TestStatusStrings[status];
    }
    return "Unknown test status code!";
}

#ifdef __cplusplus
}
#endif

#endif
