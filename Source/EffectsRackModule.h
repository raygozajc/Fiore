#pragma once

#include <JuceHeader.h>

/**
    A right-side rack for insert effects. The first slot controls the output delay.
*/
class EffectsRackModule: public juce::Component {
public:
    EffectsRackModule(juce::AudioProcessorValueTreeState& apvts);
    ~EffectsRackModule() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    void configureSlider(juce::Slider& slider,
                         juce::Label& label,
                         juce::Label& valueLabel,
                         const juce::String& labelText,
                         const juce::String& textValueSuffix,
                         int numDecimalPlacesToDisplay,
                         double defaultValue,
                         const juce::String& paramID,
                         std::unique_ptr<SliderAttachment>& attachmentToCreate);

    juce::Label moduleLabel;
    juce::Label delaySlotLabel;
    juce::TextButton delayOnButton {"On"};

    juce::Label delayTimeLabel, delayFeedbackLabel, delayMixLabel;
    juce::Label delayTimeValueLabel, delayFeedbackValueLabel, delayMixValueLabel;
    juce::Slider delayTimeSlider, delayFeedbackSlider, delayMixSlider;

    juce::Label emptySlot2, emptySlot3, emptySlot4;

    std::unique_ptr<ButtonAttachment> delayOnAttachment;
    std::unique_ptr<SliderAttachment> delayTimeAttachment, delayFeedbackAttachment, delayMixAttachment;

    juce::AudioProcessorValueTreeState& apvts;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectsRackModule)
};
