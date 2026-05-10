#include "ChorusEffect.h"

void ChorusEffect::prepare(double sampleRate, int samplesPerBlock, int outputChannels) {
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32> (outputChannels);

    chorus.prepare(spec);
    reset();
}

void ChorusEffect::reset() {
    chorus.reset();
}

void ChorusEffect::process(juce::AudioBuffer<float>& buffer, bool isOn, float rateHz, float depth, float wetMix) {
    if (!isOn) {
        return;
    }

    chorus.setRate(juce::jlimit(0.05f, 3.0f, rateHz));
    chorus.setDepth(juce::jlimit(0.0f, 0.25f, depth * 0.25f));
    chorus.setCentreDelay(8.0f);
    chorus.setFeedback(0.03f);
    chorus.setMix(juce::jlimit(0.0f, 0.45f, wetMix));

    juce::dsp::AudioBlock<float> block {buffer};
    chorus.process(juce::dsp::ProcessContextReplacing<float> (block));
}
