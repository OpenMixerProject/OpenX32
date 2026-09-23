#include "../../../dsp2/src/defines.h"
#include "test_framework.h"

#include <cmath>
#include <cstdint>
#include <cstdio>

// Production constants are the single source of truth. SHARC DM addresses are
// 32-bit word addresses; host byte counts are used only for reporting.
static const uint32_t LINKER_AUDIO_START = SDRAM_AUDIO_START_WORD_ADDRESS;
static const uint32_t LINKER_AUDIO_END   = SDRAM_AUDIO_END_WORD_ADDRESS;
static const uint32_t TOTAL_AUDIO_WORDS  = SDRAM_AUDIO_CAPACITY_WORDS;
static const uint32_t TOTAL_AUDIO_BYTES  = TOTAL_AUDIO_WORDS * 4U; // 14,680,064 bytes (14 MB)

static const int NUM_FX_SLOTS = SDRAM_AUDIO_SLOT_COUNT;

// Correct word-based sizing:
// 14MB SDRAM = 3,670,016 words.
// Sliced into 8 slots -> 3,670,016 / 8 = 458,752 words per slot (0x00070000 words = 1.75 MB).
static const uint32_t WORDS_PER_SLOT = SDRAM_AUDIO_SLOT_CAPACITY_WORDS; // 0x70000 words
static const uint32_t BYTES_PER_SLOT = WORDS_PER_SLOT * 4U;               // 1,835,008 bytes

struct FxSlotRange {
    uint32_t start_addr;
    uint32_t end_addr;
    uint32_t word_count;
};

static FxSlotRange calc_correct_slot(int slot_idx) {
    uint32_t start = LINKER_AUDIO_START + ((uint32_t)slot_idx * WORDS_PER_SLOT);
    uint32_t end   = start + WORDS_PER_SLOT - 1U;
    return { start, end, WORDS_PER_SLOT };
}

// Suspected production calculation from defines.h & fxBase.cpp:
// #define SDRAM_AUDIO_START 0x04200000
// #define SDRAM_AUDIO_SIZE_BYTE (14 * 1024 * 1024)
// _memoryAddress = (SDRAM_AUDIO_START + (_fxSlot * (SDRAM_AUDIO_SIZE_BYTE / 8)));
static const uint32_t SUSPECTED_PROD_START          = 0x04200000U;
static const uint32_t SUSPECTED_PROD_SIZE_BYTE      = 14U * 1024U * 1024U;
static const uint32_t SUSPECTED_PROD_STRIDE_WORDS   = SUSPECTED_PROD_SIZE_BYTE / 8U; // 1,835,008 (byte count used as word offset!)

static FxSlotRange calc_suspected_slot(int slot_idx) {
    uint32_t start = SUSPECTED_PROD_START + ((uint32_t)slot_idx * SUSPECTED_PROD_STRIDE_WORDS);
    uint32_t end   = start + SUSPECTED_PROD_STRIDE_WORDS - 1U;
    return { start, end, SUSPECTED_PROD_STRIDE_WORDS };
}

TEST_CASE(dsp2_memory_layout_correct_word_capacity) {
    // 1. Linker bounds check
    ASSERT_EQ(LINKER_AUDIO_START, 0x04080000U);
    ASSERT_EQ(LINKER_AUDIO_END, 0x043FFFFFU);
    ASSERT_EQ(TOTAL_AUDIO_WORDS, 0x00380000U); // 3.5 Mwords
    ASSERT_EQ(TOTAL_AUDIO_BYTES, 14U * 1024U * 1024U); // 14 MB

    // 2. Slot capacity in 32-bit SHARC words
    ASSERT_EQ(WORDS_PER_SLOT, 0x00070000U); // 458,752 words
    ASSERT_EQ(BYTES_PER_SLOT, 1835008U);     // 1.75 MB per slot
    ASSERT_EQ(WORDS_PER_SLOT * NUM_FX_SLOTS, TOTAL_AUDIO_WORDS);
}

TEST_CASE(dsp2_memory_layout_correct_eight_slots_validity) {
    FxSlotRange slots[NUM_FX_SLOTS];
    for (int i = 0; i < NUM_FX_SLOTS; i++) {
        slots[i] = calc_correct_slot(i);
    }

    // Criterion 1: First slot starts at linker-defined audio start
    ASSERT_EQ(slots[0].start_addr, LINKER_AUDIO_START);

    // Criterion 2: Slots are contiguous and completely disjoint
    for (int i = 0; i < NUM_FX_SLOTS - 1; i++) {
        ASSERT_LT(slots[i].start_addr, slots[i].end_addr);
        ASSERT_EQ(slots[i + 1].start_addr, slots[i].end_addr + 1U); // Contiguous & non-overlapping
    }

    // Criterion 3: Every slot ends at or below 0x043FFFFF
    for (int i = 0; i < NUM_FX_SLOTS; i++) {
        ASSERT_LE(slots[i].end_addr, LINKER_AUDIO_END);
    }
    // Slot 7 ends exactly at the linker boundary
    ASSERT_EQ(slots[7].end_addr, LINKER_AUDIO_END);

    // Criterion 4: Total capacity handled in 32-bit SHARC words
    uint32_t total_words = 0;
    for (int i = 0; i < NUM_FX_SLOTS; i++) {
        total_words += slots[i].word_count;
    }
    ASSERT_EQ(total_words, TOTAL_AUDIO_WORDS);

    // Exact expected address map for all 8 slots:
    uint32_t expected_starts[8] = {
        0x04080000U, 0x040F0000U, 0x04160000U, 0x041D0000U,
        0x04240000U, 0x042B0000U, 0x04320000U, 0x04390000U
    };
    uint32_t expected_ends[8] = {
        0x040EFFFFU, 0x0415FFFFU, 0x041CFFFFU, 0x0423FFFFU,
        0x042AFFFFU, 0x0431FFFFU, 0x0438FFFFU, 0x043FFFFFU
    };

    for (int i = 0; i < 8; i++) {
        ASSERT_EQ(slots[i].start_addr, expected_starts[i]);
        ASSERT_EQ(slots[i].end_addr, expected_ends[i]);
    }
}

TEST_CASE(dsp2_memory_layout_demonstrate_suspected_production_failures) {
    // Requirement 6: "the test demonstrably fails when using the current suspected
    // SDRAM_AUDIO_START and byte-count calculation"

    FxSlotRange sus_slots[NUM_FX_SLOTS];
    for (int i = 0; i < NUM_FX_SLOTS; i++) {
        sus_slots[i] = calc_suspected_slot(i);
    }

    // Failure 1: First slot does NOT start at linker audio start (0x04080000)
    // Production defines SDRAM_AUDIO_START as 0x04200000 (offset by 0x180000 words = 6MB)
    ASSERT_NE(sus_slots[0].start_addr, LINKER_AUDIO_START);
    ASSERT_EQ(sus_slots[0].start_addr, 0x04200000U);

    // Failure 2: Slot 1 already exceeds 0x043FFFFF
    // Slot 1 starts at 0x04200000 + 1,835,008 = 0x043C0000
    // Slot 1 ends at 0x043C0000 + 1,835,008 - 1 = 0x0457FFFF > 0x043FFFFF
    ASSERT_GT(sus_slots[1].end_addr, LINKER_AUDIO_END);
    ASSERT_EQ(sus_slots[1].end_addr, 0x0457FFFFU);

    // Failure 3: Slot 7 ends far beyond SDRAM hardware memory
    // Slot 7 ends at 0x04FFFFFF (overflows SDRAM by 0xC00000 = 12,582,912 words)
    ASSERT_GT(sus_slots[7].end_addr, LINKER_AUDIO_END);
    ASSERT_EQ(sus_slots[7].end_addr, 0x04FFFFFFU);

    // Failure 4: Capacity bug - treating byte size (14MB) as 32-bit word count
    // Total words attempted = 8 * (14*1024*1024 / 8) = 14,680,064 words = 58.7 MB!
    // This is 4x the total 14MB SDRAM capacity!
    uint32_t sus_total_words = 0;
    for (int i = 0; i < NUM_FX_SLOTS; i++) {
        sus_total_words += sus_slots[i].word_count;
    }
    ASSERT_EQ(sus_total_words, 14680064U);
    ASSERT_EQ(sus_total_words, TOTAL_AUDIO_WORDS * 4U); // Exactly 4x overflow!

    // Failure 5: Even if SDRAM_AUDIO_START was corrected to 0x04080000,
    // the byte-count-as-words bug alone still causes total overflow past 0x043FFFFF:
    uint32_t corrected_start_with_byte_stride_end = LINKER_AUDIO_START + (8U * SUSPECTED_PROD_STRIDE_WORDS) - 1U;
    ASSERT_GT(corrected_start_with_byte_stride_end, LINKER_AUDIO_END);
    ASSERT_EQ(corrected_start_with_byte_stride_end, 0x04E7FFFFU); // Exceeds by 0xA80000 words!
}

TEST_CASE(dsp2_memory_layout_largest_effects_fit_one_slot) {
    // fxDelay allocates two 500 ms lines at the maximum 48 kHz sample rate.
    const uint32_t delay_words = 2U * ((48000U * 500U) / 1000U);
    ASSERT_EQ(delay_words, 48000U);
    ASSERT_LE(delay_words, WORDS_PER_SLOT);

    // Independently reproduce the allocations in fxReverb's constructor:
    // four 8-channel diffusion banks followed by eight feedback delays.
    const uint32_t reverb_base_words = (48000U * 475U) / 1000U;
    uint32_t reverb_words = 0;
    for (int step = 0; step < 4; ++step) {
        reverb_words += static_cast<uint32_t>(std::ceil(
            static_cast<double>(reverb_base_words) / std::pow(2.0, step + 1))) * 8U;
    }
    for (int channel = 0; channel < 8; ++channel) {
        reverb_words += static_cast<uint32_t>(std::ceil(
            static_cast<double>(reverb_base_words) * std::pow(2.0, channel / 8.0)));
    }

    ASSERT_EQ(reverb_words, 422915U);
    ASSERT_LE(reverb_words, WORDS_PER_SLOT);
    ASSERT_EQ(WORDS_PER_SLOT - reverb_words, 35837U);
}

int main() {
    return ::HostTest::TestRegistry::instance().runAll("dsp2_memory_layout");
}
