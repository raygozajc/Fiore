#include "DistortionEffect.h"

void DistortionEffect::prepare(double sampleRate, int samplesPerBlock, int outputChannels) {
    currentSampleRate = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32> (outputChannels);

    dryBuffer.setSize(outputChannels, samplesPerBlock);
    toneFilter.prepare(spec);
    reset();
}

void DistortionEffect::reset() {
    toneFilter.reset();
}

void DistortionEffect::process(juce::AudioBuffer<float>& buffer, bool isOn, float drive, float tone, float wetMix) {
    if (!isOn) {
        return;
    }

    const auto safeDrive = juce::jlimit(0.0f, 1.0f, drive);
    const auto safeTone = juce::jlimit(0.0f, 1.0f, tone);
    const auto safeWetMix = juce::jlimit(0.0f, 0.80f, wetMix);
    const auto driveGain = juce::Decibels::decibelsToGain(juce::jmap(safeDrive, 0.0f, 1.0f, 0.0f, 36.0f));
    const auto outputTrim = juce::jmap(safeDrive, 0.0f, 1.0f, 1.0f, 0.35f);
    const auto cutoffHz = juce::jmap(safeTone, 0.0f, 1.0f, 1200.0f, 12000.0f);

    *toneFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(currentSampleRate, cutoffHz);

    dryBuffer.makeCopyOf(buffer, true);

    const auto numChannels = buffer.getNumChannels();
    const auto numSamples = buffer.getNumSamples();

    for (int channel = 0; channel < numChannels; ++channel) {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample) {
            const auto drivenSample = channelData[sample] * driveGain;
            channelData[sample] = std::tanh(drivenSample) * outputTrim;
        }
    }

    juce::dsp::AudioBlock<float> block {buffer};
    toneFilter.process(juce::dsp::ProcessContextReplacing<float> (block));

    for (int channel = 0; channel < numChannels; ++channel) {
        auto* channelData = buffer.getWritePointer(channel);
        const auto* dryData = dryBuffer.getReadPointer(channel);

        for (int sample = 0; sample < numSamples; ++sample) {
            channelData[sample] = dryData[sample] + ((channelData[sample] - dryData[sample]) * safeWetMix);
        }
    }
}
