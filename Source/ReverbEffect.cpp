#include "ReverbEffect.h"

void ReverbEffect::prepare(double sampleRate, int samplesPerBlock, int outputChannels) {
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32> (outputChannels);

    reverb.prepare(spec);
    reset();
}

void ReverbEffect::reset() {
    reverb.reset();
}

void ReverbEffect::process(juce::AudioBuffer<float>& buffer, bool isOn, float roomSize, float damping, float wetMix) {
    if (!isOn) {
        return;
    }

    const auto safeRoomSize = juce::jlimit(0.05f, 0.90f, roomSize);
    const auto safeDamping = juce::jlimit(0.0f, 0.90f, damping);
    const auto safeWetMix = juce::jlimit(0.0f, 0.50f, wetMix);

    juce::dsp::Reverb::Parameters params;
    params.roomSize = safeRoomSize;
    params.damping = safeDamping;
    params.wetLevel = safeWetMix;
    params.dryLevel = 1.0f - safeWetMix;
    params.width = 1.0f;
    params.freezeMode = 0.0f;
    reverb.setParameters(params);

    juce::dsp::AudioBlock<float> block {buffer};
    reverb.process(juce::dsp::ProcessContextReplacing<float> (block));
}
