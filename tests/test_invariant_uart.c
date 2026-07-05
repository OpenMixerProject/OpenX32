#include <check.h>
#include <stdlib.h>
#include <string.h>

/* Include the production code directly */
#include "software/x32osc/uart.c"

#ifndef OSC_BUFFER_SIZE
#define OSC_BUFFER_SIZE 256
#endif

START_TEST(test_osc_message_buffer_overflow)
{
    /* Invariant: current_length + strlen(str) must never exceed buffer size */
    const char *payloads[] = {
        /* Exact exploit: string larger than buffer */
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
        /* Boundary: exactly buffer size */
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB",
        /* Valid short input */
        "/ch/01/mix/fader",
    };
    int num_payloads = sizeof(payloads) / sizeof(payloads[0]);

    for (int i = 0; i < num_payloads; i++) {
        osc_message message;
        memset(&message, 0, sizeof(message));
        message.current_length = 0;

        size_t len = strlen(payloads[i]);
        /* The security invariant: we must not write beyond the buffer */
        ck_assert_msg(
            message.current_length + len <= sizeof(message.buffer),
            "Payload %d (len=%zu) would overflow buffer (size=%zu, offset=%u)",
            i, len, sizeof(message.buffer), message.current_length);

        /* Only perform the copy if it's safe - simulating what a fix should do */
        if (message.current_length + len <= sizeof(message.buffer)) {
            memcpy(&message.buffer[message.current_length], payloads[i], len);
            message.current_length += len;
            ck_assert_uint_le(message.current_length, sizeof(message.buffer));
        }
    }
}
END_TEST

Suite *security_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Security");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_osc_message_buffer_overflow);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = security_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}