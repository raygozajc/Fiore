#pragma once

#include <JuceHeader.h>

/**
    Output insert reverb effect used after the delay insert.
*/
class ReverbEffect {
public:
    static constexpr double tailLengthSeconds = 3.0;

    void prepare(double sampleRate, int samplesPerBlock, int outputChannels);
    void reset();
    void process(juce::AudioBuffer<float>& buffer, bool isOn, float roomSize, float damping, float wetMix);

private:
    juce::dsp::Reverb reverb;
};
