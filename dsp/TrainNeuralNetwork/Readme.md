# Neural Network Training for De-Feedback-Plugin
This folder contains a training-environment for the neural network plugin "DeFeedback" of OpenX32. This plugin allows the removal of feedback-noises within the live-application. It does not use the classical approach of an FFT to detect a peak within the frequency-band but uses a pre-trained neural network to detect feedback-noise. If this kind of noise is detected, the volume of the audio is reduced to a minimum to remove the feedback.

This is a very basic approach of such kind of plugin but it is already working and can be used as a basis for more advanced algorithms.

## Training of the neural network
The training is done using PyTorch which can be installed under Linux with the following commands:

    sudo apt install python3-torch python3-torchaudio python3-numpy 

Be aware: this will install software with more than 10 GB in size!

After the installation, prepare at least one pair of audio: one file with clean audio, and the second file with the same audio, but with disturbances. Make sure, that both files are of the same length and contain the same audio as the differences will be used for training the neural network.

Then edit the python-script and enter your filenames at the end:

    # list of training-audio-files (Dry, Disturbed)
    my_files = [
        ('audio_ref.wav', 'audio_500Hz.wav'),
        ('audio_ref.wav', 'audio_750Hz.wav'),
        ('audio_ref.wav', 'audio_1000Hz.wav'),
        ('audio_ref.wav', 'audio_2000Hz.wav')
    ]

The python-script will create a file called "coeffs.h" that will contain the necessary coefficients for the DeFeedback-plugin. Copy this file into the folder "dsp/dsp2/src/fxDeFeedback_coeffs.h" to use your new model.

## Offline-Tests
To test this plugin you can use the file "de_feedback_test.c" to test the coefficients using any linux-machine you like. After creating the file coeffs.h you can compile this software with the following command:

    gcc de_feedback_test.c -o de_feedback_test -lsndfile -lm

Then start the software with the following command:

    ./de_feedback_test inputWithFeedback.wav output.wav

If the effect is not audible make sure the audio-level is high enought as the offline-demo does not use any kind of compression/limiting like the DSP-plugin.
