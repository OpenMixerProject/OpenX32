# Makefile for OpenX32 DSP Linux-Hosted Unit & Regression Tests

CC ?= gcc
CXX ?= g++

# 32-bit compilation to match SHARC 32-bit word/pointer/long architecture
ARCH_FLAGS ?= -m32

# Base warning flags
COMMON_WARN = -Wall -Wextra -Werror -Wno-unknown-pragmas -Wno-maybe-uninitialized
CFLAGS_WARN = $(COMMON_WARN)
CXXFLAGS_WARN = $(COMMON_WARN)

INCLUDES = -Iinclude -I../../dsp1/src -I../../dsp2/src -Isrc
SHARC_COMPAT_INC = -include include/sharc_compat.h

CFLAGS = $(ARCH_FLAGS) $(CFLAGS_WARN) $(INCLUDES) $(SHARC_COMPAT_INC) -fcommon -fno-strict-aliasing -O2
CXXFLAGS = $(ARCH_FLAGS) $(CXXFLAGS_WARN) $(INCLUDES) $(SHARC_COMPAT_INC) -fno-strict-aliasing -O2
LDFLAGS = $(ARCH_FLAGS) -lm

ifeq ($(SAN),1)
CFLAGS += -fsanitize=address,undefined -fno-omit-frame-pointer
CXXFLAGS += -fsanitize=address,undefined -fno-omit-frame-pointer
LDFLAGS += -fsanitize=address,undefined
endif

BUILD_DIR = build
BIN_DIR = bin

TARGETS = $(BIN_DIR)/test_fast_approx_math \
          $(BIN_DIR)/test_helper_fcn \
          $(BIN_DIR)/test_ring_buffer \
          $(BIN_DIR)/test_dsp1_fx \
          $(BIN_DIR)/test_dsp1_block_processing \
          $(BIN_DIR)/test_dsp2_memory_layout

all: $(TARGETS)

$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@

# Production object files
$(BUILD_DIR)/fastApproxMath.o: ../../dsp2/src/fastApproxMath.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/helperFcn.o: ../../dsp2/src/helperFcn.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/buffer.o: ../../dsp2/src/buffer.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/dsp1_fx.o: ../../dsp1/src/fx.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/dsp1_audio.o: ../../dsp1/src/audio.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -Wno-comment -Wno-unused-variable -Wno-unused-parameter -c $< -o $@

$(BUILD_DIR)/dsp1_host_harness.o: src/dsp1_host_harness.c src/dsp1_host_harness.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Test binaries
$(BIN_DIR)/test_fast_approx_math: src/test_fast_approx_math.cpp $(BUILD_DIR)/fastApproxMath.o | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

$(BIN_DIR)/test_helper_fcn: src/test_helper_fcn.cpp $(BUILD_DIR)/helperFcn.o | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

$(BIN_DIR)/test_ring_buffer: src/test_ring_buffer.cpp $(BUILD_DIR)/buffer.o | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

$(BIN_DIR)/test_dsp1_fx: src/test_dsp1_fx.cpp $(BUILD_DIR)/dsp1_fx.o $(BUILD_DIR)/dsp1_audio.o $(BUILD_DIR)/dsp1_host_harness.o | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

$(BIN_DIR)/test_dsp1_block_processing: src/test_dsp1_block_processing.cpp $(BUILD_DIR)/helperFcn.o $(BUILD_DIR)/dsp1_fx.o $(BUILD_DIR)/dsp1_audio.o $(BUILD_DIR)/dsp1_host_harness.o | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

$(BIN_DIR)/test_dsp2_memory_layout: src/test_dsp2_memory_layout.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

check: all
	@for bin in $(TARGETS); do \
		$$bin || exit 1; \
	done

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all check clean
