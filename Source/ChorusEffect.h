#pragma once

#include <JuceHeader.h>

/**
    Output insert chorus effect used after the reverb insert.
*/
class ChorusEffect {
public:
    void prepare(double sampleRate, int samplesPerBlock, int outputChannels);
    void reset();
    void process(juce::AudioBuffer<float>& buffer, bool isOn, float rateHz, float depth, float wetMix);

private:
    juce::dsp::Chorus<float> chorus;
};
