#include "sim_test_framework.h"
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

void test_start(void) {
    printf("    [INFO] Program start verified on ADSP-21371 SHARC core\n");
    SIM_ASSERT_TRUE(1);
}

void test_integer_ops(void) {
    volatile int a = 12345;
    volatile int b = 6789;
    volatile int sum = a + b;
    volatile int diff = a - b;
    volatile int prod = a * b;
    volatile int quot = a / 100;
    volatile int rem = a % 100;
    volatile int bit_and = a & b;
    volatile int bit_or = a | b;
    volatile int bit_xor = a ^ b;
    volatile int bit_shift = a << 2;

    SIM_ASSERT_EQ(sum, 19134);
    SIM_ASSERT_EQ(diff, 5556);
    SIM_ASSERT_EQ(prod, 83810205);
    SIM_ASSERT_EQ(quot, 123);
    SIM_ASSERT_EQ(rem, 45);
    SIM_ASSERT_EQ(bit_and, (12345 & 6789));
    SIM_ASSERT_EQ(bit_or, (12345 | 6789));
    SIM_ASSERT_EQ(bit_xor, (12345 ^ 6789));
    SIM_ASSERT_EQ(bit_shift, (12345 << 2));
}

void test_float_ops(void) {
    volatile float fa = 3.5f;
    volatile float fb = 2.0f;
    volatile float fsum = fa + fb;
    volatile float fdiff = fa - fb;
    volatile float fprod = fa * fb;
    volatile float fquot = fa / fb;

    SIM_ASSERT_NEAR(fsum, 5.5f, 1e-5f);
    SIM_ASSERT_NEAR(fdiff, 1.5f, 1e-5f);
    SIM_ASSERT_NEAR(fprod, 7.0f, 1e-5f);
    SIM_ASSERT_NEAR(fquot, 1.75f, 1e-5f);

    volatile float f_sqrt = sqrtf(16.0f);
    SIM_ASSERT_NEAR(f_sqrt, 4.0f, 1e-5f);

    // Negative float test
    volatile float fneg = -8.25f;
    SIM_ASSERT_NEAR(fabsf(fneg), 8.25f, 1e-5f);
}

void test_type_sizes(void) {
    // SHARC ADSP-21371 has 32-bit word addressable architecture (CHAR_BIT == 32)
    // sizeof() yields size in addressable units (32-bit words)
    printf("    [SIZEOF] char:        %d word(s)\n", (int)sizeof(char));
    printf("    [SIZEOF] short:       %d word(s)\n", (int)sizeof(short));
    printf("    [SIZEOF] int:         %d word(s)\n", (int)sizeof(int));
    printf("    [SIZEOF] long:        %d word(s)\n", (int)sizeof(long));
    printf("    [SIZEOF] long long:   %d word(s)\n", (int)sizeof(long long));
    printf("    [SIZEOF] float:       %d word(s)\n", (int)sizeof(float));
    printf("    [SIZEOF] double:      %d word(s)\n", (int)sizeof(double));
    printf("    [SIZEOF] long double: %d word(s)\n", (int)sizeof(long double));
    printf("    [SIZEOF] bool:        %d word(s)\n", (int)sizeof(bool));
    printf("    [SIZEOF] void*:       %d word(s)\n", (int)sizeof(void*));

    SIM_ASSERT_EQ((int)sizeof(char), 1);
    SIM_ASSERT_EQ((int)sizeof(short), 1);
    SIM_ASSERT_EQ((int)sizeof(int), 1);
    SIM_ASSERT_EQ((int)sizeof(long), 1);
    SIM_ASSERT_EQ((int)sizeof(long long), 2);
    SIM_ASSERT_EQ((int)sizeof(float), 1);
    SIM_ASSERT_EQ((int)sizeof(double), 1);
    SIM_ASSERT_EQ((int)sizeof(long double), 2);
    SIM_ASSERT_EQ((int)sizeof(bool), 1);
    SIM_ASSERT_EQ((int)sizeof(void*), 1);
}

void test_address_arithmetic(void) {
    int i_arr[4] = { 10, 20, 30, 40 };
    float f_arr[4] = { 1.0f, 2.0f, 3.0f, 4.0f };

    int* i_p0 = &i_arr[0];
    int* i_p1 = &i_arr[1];
    float* f_p0 = &f_arr[0];
    float* f_p1 = &f_arr[1];

    // Pointer difference in typed elements
    SIM_ASSERT_EQ((int)(i_p1 - i_p0), 1);
    SIM_ASSERT_EQ((int)(f_p1 - f_p0), 1);

    // Raw character pointer difference (addressable units)
    SIM_ASSERT_EQ((int)((char*)i_p1 - (char*)i_p0), 1);
    SIM_ASSERT_EQ((int)((char*)f_p1 - (char*)f_p0), 1);

    // Absolute numeric address advances by 1 (32-bit word) per element
    uintptr_t addr0 = (uintptr_t)i_p0;
    uintptr_t addr1 = (uintptr_t)i_p1;
    SIM_ASSERT_EQ((int)(addr1 - addr0), 1);

    printf("    [ADDR] &i_arr[0] = 0x%08lx, &i_arr[1] = 0x%08lx (delta = %d word)\n",
           (unsigned long)addr0, (unsigned long)addr1, (int)(addr1 - addr0));
}

#ifdef TRIGGER_FAIL_ASSERT
void test_intentional_failure(void) {
    printf("    [FAIL_TEST] Triggering intentional assertion failure...\n");
    SIM_ASSERT_TRUE(1 == 2); // Known to fail!
}
#endif

int main(void) {
    sim_test_init();
    printf("=== ADSP-21371 CCES SIMULATOR SMOKE TEST ===\n");

    SIM_RUN_TEST(test_start);
    SIM_RUN_TEST(test_integer_ops);
    SIM_RUN_TEST(test_float_ops);
    SIM_RUN_TEST(test_type_sizes);
    SIM_RUN_TEST(test_address_arithmetic);

#ifdef TRIGGER_FAIL_ASSERT
    SIM_RUN_TEST(test_intentional_failure);
#endif

    return sim_test_summary("Smoke Test Suite");
}
