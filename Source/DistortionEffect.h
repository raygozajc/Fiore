#pragma once

#include <JuceHeader.h>

/**
    Output insert soft-clipping distortion with a post-saturation tone control.
*/
class DistortionEffect {
public:
    void prepare(double sampleRate, int samplesPerBlock, int outputChannels);
    void reset();
    void process(juce::AudioBuffer<float>& buffer, bool isOn, float drive, float tone, float wetMix);

private:
    double currentSampleRate {44100.0};
    juce::AudioBuffer<float> dryBuffer;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                   juce::dsp::IIR::Coefficients<float>> toneFilter;
};
