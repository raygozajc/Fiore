#include "FlangerEffect.h"

void FlangerEffect::prepare(double sampleRate, int samplesPerBlock, int outputChannels) {
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32> (outputChannels);

    flanger.prepare(spec);
    reset();
}

void FlangerEffect::reset() {
    flanger.reset();
}

void FlangerEffect::process(juce::AudioBuffer<float>& buffer, bool isOn, float rateHz, float depth, float wetMix) {
    if (!isOn)
        return;

    const auto safeRate = juce::jlimit(0.1f, 3.0f, rateHz);
    const auto safeDepth = juce::jlimit(0.0f, 1.0f, depth);
    const auto safeWetMix = juce::jlimit(0.0f, 0.60f, wetMix);

    flanger.setRate(safeRate);
    flanger.setDepth(juce::jlimit(0.0f, 0.025f, safeDepth * 0.025f));
    flanger.setCentreDelay(2.5f);
    flanger.setFeedback(0.45f);
    flanger.setMix(safeWetMix);

    juce::dsp::AudioBlock<float> block {buffer};
    flanger.process(juce::dsp::ProcessContextReplacing<float> (block));
}
