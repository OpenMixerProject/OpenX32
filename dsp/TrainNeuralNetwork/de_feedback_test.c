/*
	OpenMixerProject DeFeedback-Filter-Test
	https://github.com/OpenMixerProject
	30.03.2026, Chris Noeding

	# install libraries under linux
	sudo apt-get install libsndfile1-dev
	
	# install PyTorch and other things on your computer (takes around 10GB!)
	sudo apt install python3-torch python3-torchaudio python3-numpy

	# train the model
	python3 trainModel.py

	# compile and run this software
	gcc defeedback.c -o de_feedback_test -lsndfile -lm
	cp de_feedback_test /tmp/
	/tmp/de_feedback_test sample_beep.wav output.wav
	/tmp/de_feedback_test sample_beep2.wav output2.wav
*/

#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
#include <math.h>
#include <string.h>

#define TAPS 128
#define SAMPLES_IN_BUFFER 16
#define LEAKAGE 0.99995f // allow a soft leakage into the signal
#define INTERVENTION_THRESHOLD 0.95f // a threshold when the filter should fight against the feedback

// filter-coefficients from PyTorch training
#include "coeffs.h"

// filter-storages
float weights[TAPS] = {0};
float history[TAPS + SAMPLES_IN_BUFFER] = {0};

// estimate the dominant frequency of the feedback based on zero-crossings
float estimate_pitch(float *block, int n) {
    int crossings = 0;
    for (int i = 1; i < n; i++) {
        if ((block[i] > 0 && block[i-1] < 0) || (block[i] < 0 && block[i-1] > 0)) {
            crossings++;
        }
    }
    // frequency = (crossings / 2) * (SampleRate / BlockSize)
    return (crossings / 2.0f) * (48000.0f / (float)n);
}

// AI-inference: calculates the propability of a feedback within the current sample-block
float nn_inference_scalar(float input[SAMPLES_IN_BUFFER]) {
	float hidden[(SAMPLES_IN_BUFFER * 2)];
    float logit = nn_bias_out;

    // Step 1: Input -> Hidden (ReLU)
    for (int i = 0; i < (SAMPLES_IN_BUFFER * 2); i++) {
        float sum = nn_bias_h[i];
        for (int j = 0; j < SAMPLES_IN_BUFFER; j++) {
            sum += input[j] * nn_weights_ih[i][j];
        }
        hidden[i] = (sum > 0.0f) ? sum : 0.0f;
    }

    // Step 2: Hidden -> Output Logit
    for (int j = 0; j < (SAMPLES_IN_BUFFER * 2); j++) {
        logit += hidden[j] * nn_weights_ho[j];
    }

    // Step 3: Sigmoid-Activation
    float probability = 1.0f / (1.0f + expf(-logit));
    
    return probability;
}

float process(float *input, float *output) {
    // Step 1: AI decides: is it feedback (1.0) or regular audio (0.0)
    float ai_decision = nn_inference_scalar(input);
    
	// map the decision to a small learning-rate. Important: this value
	// has to be 0 when we want normal audio
    float mu_max = 0.001f; 
    float current_mu = ai_decision * mu_max;

    for (int n = 0; n < SAMPLES_IN_BUFFER; n++) {
        // Step 2: simple FIR Filter for prediction
        float pred = 0;
        for(int i = 0; i < TAPS; i++) {
            pred += weights[i] * history[i + (SAMPLES_IN_BUFFER - 1 - n)];
        }
        
        float error = input[n] - pred;
        output[n] = error;

        // Step 3: NLMS Update (Enery-Normalizing for stability)
        float energy = 0;
        for(int i = 0; i < TAPS; i++) {
            float h = history[i + (SAMPLES_IN_BUFFER - 1 - n)];
            energy += h * h;
        }
        energy += 0.01f; // small bias to prevent a DIV/0

        float step = current_mu / energy;

        // Step 4: update of the weights
        for (int i = 0; i < TAPS; i++) {
            weights[i] = (weights[i] * LEAKAGE) + step * error * history[i + (SAMPLES_IN_BUFFER - 1 - n)];
        }
    }

    // Last Step: update the history
    memmove(&history[SAMPLES_IN_BUFFER], &history[0], TAPS * sizeof(float));
    for (int i = 0; i < SAMPLES_IN_BUFFER; i++) {
        history[SAMPLES_IN_BUFFER - 1 - i] = output[i];
    }
	
	return ai_decision;
}

void process_with_resonance_penalty(float *input, float *output) {
    // Step 1: LMS Loop
    float ai_decision = process(input, output);

    // Step 2: intervention logic with a hard threshold
    if (ai_decision > INTERVENTION_THRESHOLD) {
        // we are certain that we have feedback -> fight against it
        float freq = estimate_pitch(input, SAMPLES_IN_BUFFER);

		printf("Found feedback with %f %% certainty at %f Hz\n", ai_decision * 100.0f, freq);

		// damp the output-signal to a minimum
		// TODO: use an envelope-curve here like in compressor or gate
        for (int n = 0; n < SAMPLES_IN_BUFFER; n++) {
            output[n] *= 0.1f; // Harte Absenkung um ca. 14dB während des Pfeifens
        }

		// TODO: use a notch-filter with the detected frequency to fight against feedback
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <input.wav> <output.wav>\n", argv[0]);
        return 1;
    }

    SNDFILE *infile, *outfile;
    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(sfinfo));

    if (!(infile = sf_open(argv[1], SFM_READ, &sfinfo))) return 1;
    
    // Wir erzwingen Mono für den Test-Algorithmus
    int channels = sfinfo.channels;
    outfile = sf_open(argv[2], SFM_WRITE, &sfinfo);

    float ibuf[SAMPLES_IN_BUFFER * channels];
    float obuf[SAMPLES_IN_BUFFER * channels];
    float mono_in[SAMPLES_IN_BUFFER];
    float mono_out[SAMPLES_IN_BUFFER];

    printf("Verarbeite %ld Samples...\n", (long)sfinfo.frames);

    while (sf_readf_float(infile, ibuf, SAMPLES_IN_BUFFER) > 0) {
        // Stereo-zu-Mono (falls nötig)
        for (int i = 0; i < SAMPLES_IN_BUFFER; i++) mono_in[i] = ibuf[i * channels];

        // Algorithmus aufrufen
        //process(mono_in, mono_out);
		process_with_resonance_penalty(mono_in, mono_out);

        // Mono-zu-Stereo (einfaches Kopieren)
        for (int i = 0; i < SAMPLES_IN_BUFFER; i++) {
            for (int c = 0; c < channels; c++) obuf[i * channels + c] = mono_out[i];
        }

        sf_writef_float(outfile, obuf, SAMPLES_IN_BUFFER);
    }

    sf_close(infile);
    sf_close(outfile);
    printf("Fertig! Datei gespeichert unter: %s\n", argv[2]);
    return 0;
}
