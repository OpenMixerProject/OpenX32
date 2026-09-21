#ifndef SIM_TEST_FRAMEWORK_H
#define SIM_TEST_FRAMEWORK_H

#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

// ADI runtime fatal error hook (breakpoint trigger for CCES runner)
extern void _fatal_error(void);

#ifdef __cplusplus
}
#endif

// Test runner statistics
typedef struct {
    int tests_run;
    int tests_passed;
    int tests_failed;
    int assertions_checked;
    int current_test_failed;
} SimTestContext;

static SimTestContext g_sim_ctx = { 0, 0, 0, 0, 0 };

static inline void sim_test_init(void) {
    g_sim_ctx.tests_run = 0;
    g_sim_ctx.tests_passed = 0;
    g_sim_ctx.tests_failed = 0;
    g_sim_ctx.assertions_checked = 0;
    g_sim_ctx.current_test_failed = 0;
}

static inline void sim_test_fail_action(const char* expr, const char* file, int line) {
    printf("[FAIL] Assertion failed: (%s) at %s:%d\n", expr, file, line);
    g_sim_ctx.current_test_failed = 1;
#ifdef SIM_ABORT_ON_FAIL
    printf("[ABORT] Halting execution via _fatal_error()...\n");
    _fatal_error();
#endif
}

#define SIM_ASSERT_TRUE(cond) do { \
    g_sim_ctx.assertions_checked++; \
    if (!(cond)) { \
        sim_test_fail_action(#cond, __FILE__, __LINE__); \
    } \
} while(0)

#define SIM_ASSERT_FALSE(cond) do { \
    g_sim_ctx.assertions_checked++; \
    if (cond) { \
        sim_test_fail_action("!(" #cond ")", __FILE__, __LINE__); \
    } \
} while(0)

#define SIM_ASSERT_EQ(a, b) do { \
    g_sim_ctx.assertions_checked++; \
    if ((a) != (b)) { \
        printf("[FAIL] Equality mismatch: %s != %s\n", #a, #b); \
        sim_test_fail_action(#a " == " #b, __FILE__, __LINE__); \
    } \
} while(0)

#define SIM_ASSERT_NEAR(a, b, eps) do { \
    g_sim_ctx.assertions_checked++; \
    float _diff = fabsf((float)(a) - (float)(b)); \
    if (_diff > (float)(eps)) { \
        printf("[FAIL] Near mismatch: |%s - %s| = %f > eps (%f)\n", #a, #b, _diff, (float)(eps)); \
        sim_test_fail_action("|" #a " - " #b "| <= " #eps, __FILE__, __LINE__); \
    } \
} while(0)

#define SIM_RUN_TEST(func) do { \
    g_sim_ctx.tests_run++; \
    g_sim_ctx.current_test_failed = 0; \
    printf("--> Running %s...\n", #func); \
    func(); \
    if (g_sim_ctx.current_test_failed) { \
        g_sim_ctx.tests_failed++; \
        printf("    [RESULT] %s FAILED\n", #func); \
    } else { \
        g_sim_ctx.tests_passed++; \
        printf("    [RESULT] %s PASSED\n", #func); \
    } \
} while(0)

static inline int sim_test_summary(const char* suite_name) {
    printf("\n========================================\n");
    printf("TEST SUITE SUMMARY: %s\n", suite_name);
    printf("Tests Run:    %d\n", g_sim_ctx.tests_run);
    printf("Tests Passed: %d\n", g_sim_ctx.tests_passed);
    printf("Tests Failed: %d\n", g_sim_ctx.tests_failed);
    printf("Assertions:   %d\n", g_sim_ctx.assertions_checked);
    printf("========================================\n");
    if (g_sim_ctx.tests_failed > 0) {
        printf("RESULT: FAILED (%d tests failed)\n", g_sim_ctx.tests_failed);
        _fatal_error();
        return 1;
    } else {
        printf("RESULT: ALL PASS\n");
        return 0;
    }
}

#endif /* SIM_TEST_FRAMEWORK_H */
