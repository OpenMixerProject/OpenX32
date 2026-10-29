/*
  Host unit tests for DSP1. Each test drives the DSP the same way the i.MX25 does
  (commExecCommand() with SPI class-ids) and runs the real audioProcessData().

  Build and run: make -C dsp/dsp1/test
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dsp1.h"
#include "audio.h"
#include "comm.h"

extern int audioRxBuf[];
extern float (*delayLineInput)[SAMPLES_IN_DELAYLINE];
extern float (*delayLineOutput)[SAMPLES_IN_DELAYLINE];

static float hostDelayLineInput[MAX_CHAN_FPGA][SAMPLES_IN_DELAYLINE];
static float hostDelayLineOutput[MAX_CHAN_FPGA][SAMPLES_IN_DELAYLINE];
static float sendGains[MAX_CHAN_FPGA + MAX_DSP2_FXRETURN][MAX_MIXBUS];

static int failures = 0;
static const char* currentTest = "";

#define CHECK_NEAR(actual, expected) checkNear((actual), (expected), #actual, __LINE__)

static void checkNear(float actual, float expected, const char* expr, int line) {
	if (fabsf(actual - expected) > 0.01f) {
		printf("  FAIL %s (line %d): %s = %f, expected %f\n", currentTest, line, expr, actual, expected);
		failures++;
	}
}

// ---------------------------------------------------------------------------
// helpers that talk to the DSP like the i.MX25 does
// ---------------------------------------------------------------------------

static void setFader(int ch, float volume) {
	float values[5] = {1.0f, volume, 0.0f, 0.0f, 0.0f}; // trim, volume, main L, main R, sub
	commExecCommand('v', ch, 0, 5, values);
	dsp.channelVolume[ch] = volume; // skip the 333us volume smoothing
}

static void setSend(int ch, int bus, float gain) {
	sendGains[ch][bus] = gain;
	commExecCommand('s', ch, 0, MAX_MIXBUS, &sendGains[ch][0]);
}

static void setSendTapPoint(int ch, int bus, int tap) {
	unsigned int values[2] = {bus, tap};
	commExecCommand('t', ch, 0, 2, values);
}

static void setupMixer(void) {
	memset(&dsp, 0, sizeof(dsp));
	memset(audioBuffer, 0, sizeof(audioBuffer));
	memset(sendGains, 0, sizeof(sendGains));
	delayLineInput = hostDelayLineInput;
	delayLineOutput = hostDelayLineOutput;
	audioInit();

	for (int i = 0; i < MAX_CHAN_FPGA + MAX_CHAN_DSP2; i++) {
		dsp.outputSourcePtr[i] = &audioBuffer[TAP_INPUT][DSP_BUF_IDX_OFF][0];
	}

	for (int ch = 0; ch < MAX_CHAN_FPGA; ch++) {
		unsigned int inputRouting[2] = {DSP_BUF_IDX_DSPCHANNEL + ch, TAP_INPUT}; // 1:1 routing
		commExecCommand('r', ch, 0, 2, inputRouting);

		float lowcut = 1.0f; // single-pole lowcut with coeff 1.0 passes the signal unchanged
		commExecCommand('e', ch, 'l', 1, &lowcut);

		// threshold far above the signal, makeup 1, instant attack/release -> unity gain
		float compressor[6] = {200.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
		commExecCommand('c', ch, 0, 6, compressor);

		setFader(ch, 1.0f);
	}
}

static void feedConstant(int ch, int value) {
	int i_tdm = ch / CHANNELS_PER_TDM;
	for (int half = 0; half < BUFFER_COUNT; half++) {
		for (int s = 0; s < SAMPLES_IN_BUFFER; s++) {
			audioRxBuf[(BUFFER_COUNT * BUFFER_SIZE * i_tdm) + (BUFFER_SIZE * half) + (s * CHANNELS_PER_TDM) + (ch % CHANNELS_PER_TDM)] = value;
		}
	}
}

static void runBlocks(int count) {
	for (int i = 0; i < count; i++) {
		audioProcessData();
	}
}

static float busSample(int bus) {
	return audioBuffer[TAP_INPUT][DSP_BUF_IDX_MIXBUS + bus][SAMPLES_IN_BUFFER - 1];
}

// coefficients of one biquad section as the i.MX25 sends them: a0 a1 a2 -b1 -b2
static const float passthrough[5] = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f};

static void setAltEq(int ch, bool enable, const float firstBand[5]) {
	// same layout as DSP1::SendEQ(): sections 0/1 interleaved, odd last section plain
	float values[(5 * EQ_ALT_BANDS) + 1];
	for (int band = 0; band < EQ_ALT_BANDS; band++) {
		const float* c = (band == 0) ? firstBand : passthrough;
		for (int i = 0; i < 5; i++) {
			if (((EQ_ALT_BANDS % 2) == 0) || (band < (EQ_ALT_BANDS - 1))) {
				values[((band / 2) * 2) * 5 + (band % 2) + (2 * i)] = c[i];
			} else {
				values[(EQ_ALT_BANDS - 1) * 5 + i] = c[i];
			}
		}
	}
	values[5 * EQ_ALT_BANDS] = enable ? 1.0f : 0.0f;
	commExecCommand('e', ch, 'a', (5 * EQ_ALT_BANDS) + 1, values);
}

static const float gainOf2[5] = {2.0f, 0.0f, 0.0f, 0.0f, 0.0f};

// ---------------------------------------------------------------------------
// tests
// ---------------------------------------------------------------------------

static void test_harness_delivers_channel_signal_to_post_fader_tap(void) {
	setupMixer();
	feedConstant(3, 1000);
	setFader(3, 0.5f);

	runBlocks(1);

	CHECK_NEAR(audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_DSPCHANNEL + 3][SAMPLES_IN_BUFFER - 1], 500.0f);
}

static void test_send_defaults_to_pre_fader(void) {
	setupMixer();
	feedConstant(0, 1000);
	setFader(0, 0.5f);
	setSend(0, 0, 1.0f);

	runBlocks(1);

	CHECK_NEAR(busSample(0), 1000.0f);
}

static void test_each_bus_has_its_own_tap_point(void) {
	setupMixer();
	feedConstant(0, 1000);
	setFader(0, 0.5f);
	setSend(0, 0, 1.0f);
	setSend(0, 1, 1.0f);
	setSendTapPoint(0, 0, TAP_PRE_FADER);
	setSendTapPoint(0, 1, TAP_POST_FADER);

	runBlocks(1);

	CHECK_NEAR(busSample(0), 1000.0f);
	CHECK_NEAR(busSample(1), 500.0f);
}

static void test_tap_points_in_the_same_bus_group_are_independent(void) {
	// buses 0..3 are processed together; mix taps inside one group of four
	setupMixer();
	feedConstant(0, 1000);
	setFader(0, 0.25f);
	for (int bus = 0; bus < 4; bus++) {
		setSend(0, bus, 1.0f);
	}
	setSendTapPoint(0, 0, TAP_POST_FADER);
	setSendTapPoint(0, 1, TAP_PRE_FADER);
	setSendTapPoint(0, 2, TAP_INPUT);
	setSendTapPoint(0, 3, TAP_POST_FADER);

	runBlocks(1);

	CHECK_NEAR(busSample(0), 250.0f);
	CHECK_NEAR(busSample(1), 1000.0f);
	CHECK_NEAR(busSample(2), 1000.0f);
	CHECK_NEAR(busSample(3), 250.0f);
}

static void test_bus_sums_several_channels_with_different_taps(void) {
	setupMixer();
	feedConstant(0, 1000);
	feedConstant(1, 100);
	setFader(0, 0.5f);
	setFader(1, 0.5f);
	setSend(0, 5, 1.0f);
	setSend(1, 5, 2.0f);
	setSendTapPoint(0, 5, TAP_POST_FADER);
	setSendTapPoint(1, 5, TAP_PRE_FADER);

	runBlocks(1);

	CHECK_NEAR(busSample(5), 500.0f + 200.0f);
}

static void test_bus_does_not_accumulate_across_blocks(void) {
	setupMixer();
	feedConstant(0, 1000);
	setSend(0, 0, 1.0f);

	runBlocks(3);

	CHECK_NEAR(busSample(0), 1000.0f);
}

static void test_channel_pre_fader_signal_survives_mixbus_processing(void) {
	setupMixer();
	feedConstant(0, 1000);
	setSend(0, 0, 1.0f);

	runBlocks(1);

	CHECK_NEAR(audioBuffer[TAP_PRE_FADER][DSP_BUF_IDX_DSPCHANNEL + 0][SAMPLES_IN_BUFFER - 1], 1000.0f);
}

static void test_invalid_tap_point_commands_are_ignored(void) {
	setupMixer();
	feedConstant(0, 1000);
	setFader(0, 0.5f);
	setSend(0, 0, 1.0f);
	setSendTapPoint(0, 0, TAP_POST_FADER);
	setSendTapPoint(0, 0, 99);          // unknown tap point
	setSendTapPoint(0, MAX_MIXBUS, 0);  // unknown bus

	runBlocks(1);

	CHECK_NEAR(busSample(0), 500.0f);
}

static void test_alt_eq_tap_applies_occlusion_eq_only_on_that_send(void) {
	setupMixer();
	feedConstant(0, 1000);
	setFader(0, 0.5f);
	setAltEq(0, true, gainOf2);
	setSend(0, 2, 1.0f);
	setSend(0, 3, 1.0f);
	setSendTapPoint(0, 2, TAP_ALT_EQ);
	setSendTapPoint(0, 3, TAP_PRE_FADER);

	runBlocks(1);

	CHECK_NEAR(busSample(2), 2000.0f);
	CHECK_NEAR(busSample(3), 1000.0f);
	CHECK_NEAR(audioBuffer[TAP_PRE_FADER][DSP_BUF_IDX_DSPCHANNEL + 0][SAMPLES_IN_BUFFER - 1], 1000.0f);
}

static void test_alt_eq_tap_falls_back_to_pre_fader_without_alt_eq(void) {
	setupMixer();
	feedConstant(0, 1000);
	setFader(0, 0.5f);
	setSend(0, 2, 1.0f);
	setSendTapPoint(0, 2, TAP_ALT_EQ);

	runBlocks(1);

	CHECK_NEAR(busSample(2), 1000.0f);
}

static void test_enabling_alt_eq_after_setting_the_tap_takes_effect(void) {
	setupMixer();
	feedConstant(0, 1000);
	setSend(0, 2, 1.0f);
	setSendTapPoint(0, 2, TAP_ALT_EQ);
	setAltEq(0, true, gainOf2);

	runBlocks(1);

	CHECK_NEAR(busSample(2), 2000.0f);
}

static void test_disabling_alt_eq_falls_back_to_pre_fader(void) {
	setupMixer();
	feedConstant(0, 1000);
	setSend(0, 2, 1.0f);
	setAltEq(0, true, gainOf2);
	setSendTapPoint(0, 2, TAP_ALT_EQ);
	setAltEq(0, false, gainOf2);

	runBlocks(1);

	CHECK_NEAR(busSample(2), 1000.0f);
}

static void test_alt_eq_keeps_filter_state_across_blocks(void) {
	// one-pole lowpass y = x + 0.5 * y has a DC gain of 2 once it has settled
	static const float onePole[5] = {1.0f, 0.0f, 0.0f, 0.5f, 0.0f};
	setupMixer();
	feedConstant(0, 1000);
	setSend(0, 2, 1.0f);
	setSendTapPoint(0, 2, TAP_ALT_EQ);
	setAltEq(0, true, onePole);

	runBlocks(4);

	CHECK_NEAR(busSample(2), 2000.0f);
}

static void test_alt_eq_is_limited_to_its_slots(void) {
	setupMixer();
	for (int ch = 0; ch <= MAX_ALTEQ_SLOTS; ch++) {
		feedConstant(ch, 1000);
		setAltEq(ch, true, gainOf2);
		setSendTapPoint(ch, ch, TAP_ALT_EQ);
		setSend(ch, ch, 1.0f);
	}

	runBlocks(1);

	CHECK_NEAR(busSample(0), 2000.0f);
	CHECK_NEAR(busSample(MAX_ALTEQ_SLOTS - 1), 2000.0f);
	CHECK_NEAR(busSample(MAX_ALTEQ_SLOTS), 1000.0f); // no slot left -> pre-fader

	// freeing a slot lets the next channel use it
	setAltEq(0, false, gainOf2);
	setAltEq(MAX_ALTEQ_SLOTS, true, gainOf2);

	runBlocks(1);

	CHECK_NEAR(busSample(0), 1000.0f);
	CHECK_NEAR(busSample(MAX_ALTEQ_SLOTS), 2000.0f);
}

static void test_updating_alt_eq_coefficients_keeps_the_slot(void) {
	static const float gainOf3[5] = {3.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	setupMixer();
	feedConstant(0, 1000);
	setSend(0, 2, 1.0f);
	setSendTapPoint(0, 2, TAP_ALT_EQ);
	setAltEq(0, true, gainOf2);
	setAltEq(0, true, gainOf3);
	for (int ch = 1; ch < MAX_ALTEQ_SLOTS; ch++) {
		setAltEq(ch, true, gainOf2); // fills all remaining slots
	}

	runBlocks(1);

	CHECK_NEAR(busSample(2), 3000.0f);
}

static void test_alt_eq_on_fx_return_is_ignored(void) {
	setupMixer();
	setAltEq(MAX_CHAN_FPGA, true, gainOf2);
	setSendTapPoint(MAX_CHAN_FPGA, 0, TAP_ALT_EQ);
	setSend(MAX_CHAN_FPGA, 0, 1.0f);

	runBlocks(1);

	CHECK_NEAR(busSample(0), 0.0f);
}

static void test_routing_rejects_tap_points_without_audio_buffer(void) {
	setupMixer();
	unsigned int valid[2] = {DSP_BUF_IDX_MAINLEFT, TAP_POST_FADER};
	unsigned int altEq[2] = {DSP_BUF_IDX_MAINLEFT, TAP_ALT_EQ};
	commExecCommand('r', 0, 1, 2, valid);
	commExecCommand('r', 0, 1, 2, altEq);
	commExecCommand('r', 1, 0, 2, altEq);

	CHECK_NEAR((float)(dsp.outputSourcePtr[0] == &audioBuffer[TAP_POST_FADER][DSP_BUF_IDX_MAINLEFT][0]), 1.0f);
	CHECK_NEAR((float)(dsp.inputSourcePtr[1] == &audioBuffer[TAP_INPUT][DSP_BUF_IDX_DSPCHANNEL + 1][0]), 1.0f);
}

static void test_matrix_send_rejects_tap_points_without_audio_buffer(void) {
	setupMixer();
	unsigned int busToMatrix[2] = {0, TAP_POST_FADER};  // matrix 0, tap point
	commExecCommand('t', 0, 1, 2, busToMatrix);
	unsigned int mainToMatrix[2] = {0, TAP_POST_FADER};
	commExecCommand('t', 0, 2, 2, mainToMatrix);

	busToMatrix[1] = TAP_ALT_EQ;
	commExecCommand('t', 0, 1, 2, busToMatrix);
	mainToMatrix[1] = TAP_ALT_EQ;
	commExecCommand('t', 0, 2, 2, mainToMatrix);

	CHECK_NEAR(dsp.sendMatrixTapPoint[0][0], TAP_POST_FADER);
	CHECK_NEAR(dsp.sendMatrixTapPoint[0][MAX_MIXBUS], TAP_POST_FADER);
}

#define RUN(test) do { currentTest = #test; int before = failures; test(); printf("%s %s\n", (failures == before) ? "ok  " : "FAIL", #test); } while (0)

int main(void) {
	RUN(test_harness_delivers_channel_signal_to_post_fader_tap);
	RUN(test_send_defaults_to_pre_fader);
	RUN(test_each_bus_has_its_own_tap_point);
	RUN(test_tap_points_in_the_same_bus_group_are_independent);
	RUN(test_bus_sums_several_channels_with_different_taps);
	RUN(test_bus_does_not_accumulate_across_blocks);
	RUN(test_channel_pre_fader_signal_survives_mixbus_processing);
	RUN(test_invalid_tap_point_commands_are_ignored);
	RUN(test_alt_eq_tap_applies_occlusion_eq_only_on_that_send);
	RUN(test_alt_eq_tap_falls_back_to_pre_fader_without_alt_eq);
	RUN(test_enabling_alt_eq_after_setting_the_tap_takes_effect);
	RUN(test_disabling_alt_eq_falls_back_to_pre_fader);
	RUN(test_alt_eq_keeps_filter_state_across_blocks);
	RUN(test_alt_eq_is_limited_to_its_slots);
	RUN(test_updating_alt_eq_coefficients_keeps_the_slot);
	RUN(test_alt_eq_on_fx_return_is_ignored);
	RUN(test_routing_rejects_tap_points_without_audio_buffer);
	RUN(test_matrix_send_rejects_tap_points_without_audio_buffer);

	printf("\n%s (%d failure%s)\n", failures ? "FAILED" : "PASSED", failures, failures == 1 ? "" : "s");
	return failures ? 1 : 0;
}
