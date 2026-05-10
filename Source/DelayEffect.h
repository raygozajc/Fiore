#pragma once

#include <JuceHeader.h>

/**
    Output insert delay effect used after the synthesiser mix.
*/
class DelayEffect {
public:
    static constexpr float maxDelayTimeSeconds = 2.0f;

    void prepare(double sampleRate, int samplesPerBlock, int outputChannels);
    void reset();
    void process(juce::AudioBuffer<float>& buffer, bool isOn, float delayTimeMs, float feedback, float wetMix);

private:
    double currentSampleRate {44100.0};
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLine;
};
