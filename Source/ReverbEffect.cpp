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

    juce::dsp::Reverb::Parameters params;
    params.roomSize = roomSize;
    params.damping = damping;
    params.wetLevel = wetMix;
    params.dryLevel = 1.0f - wetMix;
    params.width = 1.0f;
    params.freezeMode = 0.0f;
    reverb.setParameters(params);

    juce::dsp::AudioBlock<float> block {buffer};
    reverb.process(juce::dsp::ProcessContextReplacing<float> (block));
}
