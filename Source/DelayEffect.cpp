#include "DelayEffect.h"

void DelayEffect::prepare(double sampleRate, int samplesPerBlock, int outputChannels) {
    currentSampleRate = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32> (outputChannels);

    delayLine.setMaximumDelayInSamples(static_cast<int> (std::ceil(maxDelayTimeSeconds * sampleRate)));
    delayLine.prepare(spec);
    reset();
}

void DelayEffect::reset() {
    delayLine.reset();
}

void DelayEffect::process(juce::AudioBuffer<float>& buffer, bool isOn, float delayTimeMs, float feedback, float wetMix) {
    delayLine.setDelay(delayTimeMs * 0.001f * static_cast<float> (currentSampleRate));

    const auto numChannels = buffer.getNumChannels();
    const auto numSamples = buffer.getNumSamples();

    for (int channel = 0; channel < numChannels; ++channel) {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample) {
            const auto drySample = channelData[sample];
            const auto delayedSample = delayLine.popSample(channel);

            if (isOn) {
                delayLine.pushSample(channel, drySample + delayedSample * feedback);
                channelData[sample] = drySample + delayedSample * wetMix;
            } else {
                delayLine.pushSample(channel, 0.0f);
            }
        }
    }
}
