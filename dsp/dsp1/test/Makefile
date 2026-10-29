# Host build of the DSP1 audio code for unit tests (no CrossCore Embedded Studio needed).
# Usage: make -C dsp/dsp1/test

CC ?= cc
SRC = ../src
CFLAGS = -std=gnu99 -O1 -g -fcommon -I stubs -I $(SRC) \
         -Wall -Wno-unknown-pragmas -Wno-unused-variable -Wno-unused-but-set-variable \
         -Wno-pointer-bool-conversion -Wno-address -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast \
         -Wno-ignored-pragmas -Wno-macro-redefined

OBJS = test_dsp1.c host_support.c $(SRC)/audio.c $(SRC)/comm.c $(SRC)/fx.c

test: test_dsp1
	./test_dsp1

test_dsp1: $(OBJS) $(wildcard $(SRC)/*.h) $(wildcard stubs/*.h)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -lm

clean:
	rm -f test_dsp1

.PHONY: test clean
