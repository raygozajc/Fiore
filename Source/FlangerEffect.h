#pragma once

#include <JuceHeader.h>

/**
    Output insert phaser/flanger effect used after the chorus insert.
*/
class FlangerEffect {
public:
    void prepare(double sampleRate, int samplesPerBlock, int outputChannels);
    void reset();
    void process(juce::AudioBuffer<float>& buffer, bool isOn, float rateHz, float depth, float wetMix);

private:
    juce::dsp::Chorus<float> flanger;
};
