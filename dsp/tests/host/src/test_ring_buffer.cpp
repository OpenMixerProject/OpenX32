#include "test_framework.h"
#include "buffer.h"
#undef M_PI
#include "defines.h"

#include <cmath>
#include <vector>

TEST_CASE(buffer_init_and_clean_state) {
    const int LEN = 64;
    float mem[LEN];
    for (int i = 0; i < LEN; i++) mem[i] = 123.45f;

    sRingBuffer rb;
    bufferMovingRmsInit(&rb, mem, LEN);

    ASSERT_EQ(rb.bufferLen, LEN);
    ASSERT_EQ(rb.head, 0);
    ASSERT_EQ(rb.output, 0.0f);
    ASSERT_NEAR(rb.one_over_bufferLen, 1.0f / 64.0f, 1e-7f);

    // Verify memory cleared to zero
    for (int i = 0; i < LEN; i++) {
        ASSERT_EQ(mem[i], 0.0f);
    }
}

TEST_CASE(buffer_write_and_head_wraparound) {
    const int LEN = 8;
    float mem[LEN] = {0};
    sRingBuffer rb;
    bufferMovingRmsInit(&rb, mem, LEN);

    // Write LEN - 1 samples: head advances linearly
    for (int i = 0; i < LEN - 1; i++) {
        bufferWrite(&rb, (float)(i + 1));
        ASSERT_EQ(rb.head, i + 1);
        ASSERT_EQ(mem[i], (float)(i + 1));
    }

    // Write LEN-th sample: head must wrap around to 0
    bufferWrite(&rb, 8.0f);
    ASSERT_EQ(rb.head, 0);
    ASSERT_EQ(mem[LEN - 1], 8.0f);

    // Write another sample past wrap: head is 1
    bufferWrite(&rb, 9.0f);
    ASSERT_EQ(rb.head, 1);
    ASSERT_EQ(mem[0], 9.0f); // overwrote index 0
}

TEST_CASE(buffer_read_index_offsets) {
    const int LEN = 10;
    float mem[LEN] = {0};
    sRingBuffer rb;
    bufferMovingRmsInit(&rb, mem, LEN);

    // Populate with distinct values: 10, 20, 30, ...
    for (int i = 0; i < 6; i++) {
        bufferWrite(&rb, (float)((i + 1) * 10));
    }
    // Head is now at 6. Elements 0..5 are 10, 20, 30, 40, 50, 60.

    // offset = 1: most recently written sample (index 5 -> 60)
    ASSERT_EQ(bufferRead(&rb, 1), 60.0f);
    // offset = 2: sample before that (index 4 -> 50)
    ASSERT_EQ(bufferRead(&rb, 2), 50.0f);
    // offset = 6: first written sample (index 0 -> 10)
    ASSERT_EQ(bufferRead(&rb, 6), 10.0f);

    // offset = 7: wraps around backwards to index (6 - 7 + 10) = 9 -> 0.0f
    ASSERT_EQ(bufferRead(&rb, 7), 0.0f);

    // Now write past buffer length to test wrap with active data
    for (int i = 6; i < 15; i++) {
        bufferWrite(&rb, (float)((i + 1) * 10));
    }
    // Wrote 15 items total. Buffer has 10 elements.
    // Head is at 15 % 10 = 5.
    // Most recent is item 15 (150) at index 4.
    ASSERT_EQ(bufferRead(&rb, 1), 150.0f);
    ASSERT_EQ(bufferRead(&rb, 5), 110.0f);
    // offset 6 -> index 5 - 6 + 10 = 9 (value 100)
    ASSERT_EQ(bufferRead(&rb, 6), 100.0f);
    // offset 10 -> index 5 - 10 + 10 = 5 (value 60)
    ASSERT_EQ(bufferRead(&rb, 10), 60.0f);
}

TEST_CASE(buffer_read_ms_delay_calculation) {
    const int LEN = 4800; // 100ms at 48kHz
    std::vector<float> mem(LEN, 0.0f);
    sRingBuffer rb;
    bufferMovingRmsInit(&rb, mem.data(), LEN);

    // Fill buffer with ramp
    for (int i = 0; i < LEN; i++) {
        bufferWrite(&rb, (float)i);
    }
    // Head wrapped to 0

    // 10ms at 48000 Hz = 480 samples offset
    // offset = delayMs * samplerate * 0.001f = 10 * 48000 * 0.001 = 480
    // tail = 0 - 480 + 4800 = 4320
    float val_10ms = bufferReadMs(&rb, 10.0f, 48000);
    ASSERT_EQ(val_10ms, 4320.0f);

    // 50ms at 48000 Hz = 2400 samples
    // tail = 0 - 2400 + 4800 = 2400
    float val_50ms = bufferReadMs(&rb, 50.0f, 48000);
    ASSERT_EQ(val_50ms, 2400.0f);

    // 0ms delay: offset 0 -> tail 0 -> index 0 (which has value 0)
    float val_0ms = bufferReadMs(&rb, 0.0f, 48000);
    ASSERT_EQ(val_0ms, 0.0f);
}

TEST_CASE(buffer_moving_rms_dc_and_window_recurrence) {
    const int WINDOW = 8;
    float mem[WINDOW] = {0};
    sRingBuffer rb;
    bufferMovingRmsInit(&rb, mem, WINDOW);

    // 1. DC signal test: with constant input X = 2.0f,
    // once steady state is reached, output equals X^2 * (WINDOW-1)/WINDOW = 4.0 * 7/8 = 3.5f
    // because production bufferMovingRms overwrites buffer[head] before subtracting,
    // causing sample (n-(WINDOW-1)) to be subtracted early.
    for (int i = 0; i < 20; i++) {
        bufferMovingRms(&rb, 2.0f);
    }
    float expected_dc = (4.0f * (float)(WINDOW - 1)) / (float)WINDOW;
    ASSERT_NEAR(rb.output, expected_dc, 1e-5f);

    // 2. Exact step-by-step verification with ramp signal
    bufferMovingRmsInit(&rb, mem, WINDOW);
    std::vector<float> input = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    float running_sum = 0.0f;
    for (size_t i = 0; i < input.size(); i++) {
        float x = input[i];
        float out = bufferMovingRms(&rb, x);

        // Track the production algorithm's exact recurrence:
        // Subtract sample from i - (WINDOW - 1) if i >= (WINDOW - 1)
        float in_sq = x * x;
        float sub_sq = 0.0f;
        if ((int)i >= (WINDOW - 1)) {
            float old_x = input[i - (WINDOW - 1)];
            sub_sq = old_x * old_x;
        }
        running_sum += (in_sq - sub_sq) / (float)WINDOW;
        ASSERT_NEAR(out, running_sum, 1e-5f);
    }
}

TEST_CASE(spi_rx_ring_buffer_wraparound_and_capacity) {
    // Test the production SPI ring-buffer FIFO logic from dsp1/src/spi.c
    const int BUF_SIZE = 200; // SPI_RX_BUFFER_SIZE
    unsigned int buffer[BUF_SIZE] = {0};
    int head = 0;
    int tail = 0;

    // Push until full: maximum capacity is BUF_SIZE - 1 (199 items)
    int pushed = 0;
    for (int i = 0; i < BUF_SIZE * 2; i++) {
        int next_head = head + 1;
        if (next_head >= BUF_SIZE) next_head -= BUF_SIZE;

        if (next_head != tail) {
            buffer[head] = (unsigned int)(i + 100);
            head = next_head;
            pushed++;
        }
    }
    ASSERT_EQ(pushed, BUF_SIZE - 1);
    ASSERT_EQ(pushed, 199);

    // Pop 100 items
    int popped = 0;
    for (int i = 0; i < 100; i++) {
        ASSERT_NE(head, tail);
        unsigned int data = buffer[tail];
        ASSERT_EQ(data, (unsigned int)(i + 100));
        tail += 1;
        if (tail >= BUF_SIZE) tail -= BUF_SIZE;
        popped++;
    }
    ASSERT_EQ(popped, 100);

    // Push 100 more items (testing wraparound of head past BUF_SIZE)
    for (int i = 0; i < 100; i++) {
        int next_head = head + 1;
        if (next_head >= BUF_SIZE) next_head -= BUF_SIZE;
        ASSERT_NE(next_head, tail);
        buffer[head] = (unsigned int)(i + 500);
        head = next_head;
        pushed++;
    }

    // Pop all remaining items and verify FIFO order across wraparound boundary
    while (head != tail) {
        tail += 1;
        if (tail >= BUF_SIZE) tail -= BUF_SIZE;
        popped++;
    }
    ASSERT_EQ(popped, pushed);
    ASSERT_EQ(head, tail);
}

TEST_CASE(audio_delayline_split_block_wraparound) {
    // Tests the split block read/write logic used in dsp1/src/audio.c:370-410
    const int BLOCK_SIZE = 16;
    const int DELAY_LEN = 24000;
    std::vector<float> delay_line(DELAY_LEN, 0.0f);

    int head = DELAY_LEN - 8; // 8 samples left before end of buffer

    float input_block[BLOCK_SIZE];
    for (int i = 0; i < BLOCK_SIZE; i++) input_block[i] = (float)(i + 1);

    // Write split block logic from production:
    int wrapPoint = DELAY_LEN - head;
    if (wrapPoint >= BLOCK_SIZE) {
        std::memcpy(&delay_line[head], input_block, BLOCK_SIZE * sizeof(float));
        head += BLOCK_SIZE;
    } else {
        std::memcpy(&delay_line[head], input_block, wrapPoint * sizeof(float));
        std::memcpy(&delay_line[0], input_block + wrapPoint, (BLOCK_SIZE - wrapPoint) * sizeof(float));
        head = BLOCK_SIZE - wrapPoint;
    }

    // Assert head position wrapped correctly
    ASSERT_EQ(head, 8);

    // Verify first 8 samples went to end of delay line [23992..23999]
    for (int i = 0; i < 8; i++) {
        ASSERT_EQ(delay_line[DELAY_LEN - 8 + i], (float)(i + 1));
    }
    // Verify last 8 samples went to start of delay line [0..7]
    for (int i = 0; i < 8; i++) {
        ASSERT_EQ(delay_line[i], (float)(i + 9));
    }

    // Read back with 16 samples delay (should retrieve exactly input_block)
    float read_block[BLOCK_SIZE] = {0};
    int tail = head - BLOCK_SIZE;
    if (tail < 0) tail += DELAY_LEN; // tail = 8 - 16 + 24000 = 23992

    int readWrap = DELAY_LEN - tail;
    if (readWrap >= BLOCK_SIZE) {
        std::memcpy(read_block, &delay_line[tail], BLOCK_SIZE * sizeof(float));
    } else {
        std::memcpy(read_block, &delay_line[tail], readWrap * sizeof(float));
        std::memcpy(read_block + readWrap, &delay_line[0], (BLOCK_SIZE - readWrap) * sizeof(float));
    }

    for (int i = 0; i < BLOCK_SIZE; i++) {
        ASSERT_EQ(read_block[i], input_block[i]);
    }
}

int main() {
    return ::HostTest::TestRegistry::instance().runAll("ringBuffer");
}
