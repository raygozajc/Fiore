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
    juce::Label reverbSlotLabel;
    juce::TextButton reverbOnButton {"On"};
    juce::Label chorusSlotLabel;
    juce::TextButton chorusOnButton {"On"};
    juce::Label distortionSlotLabel;
    juce::TextButton distortionOnButton {"Off"};

    juce::Label delayTimeLabel, delayFeedbackLabel, delayMixLabel;
    juce::Label delayTimeValueLabel, delayFeedbackValueLabel, delayMixValueLabel;
    juce::Slider delayTimeSlider, delayFeedbackSlider, delayMixSlider;

    juce::Label reverbRoomLabel, reverbDampingLabel, reverbMixLabel;
    juce::Label reverbRoomValueLabel, reverbDampingValueLabel, reverbMixValueLabel;
    juce::Slider reverbRoomSlider, reverbDampingSlider, reverbMixSlider;

    juce::Label chorusRateLabel, chorusDepthLabel, chorusMixLabel;
    juce::Label chorusRateValueLabel, chorusDepthValueLabel, chorusMixValueLabel;
    juce::Slider chorusRateSlider, chorusDepthSlider, chorusMixSlider;

    juce::Label distortionDriveLabel, distortionToneLabel, distortionMixLabel;
    juce::Label distortionDriveValueLabel, distortionToneValueLabel, distortionMixValueLabel;
    juce::Slider distortionDriveSlider, distortionToneSlider, distortionMixSlider;

    std::unique_ptr<ButtonAttachment> delayOnAttachment;
    std::unique_ptr<SliderAttachment> delayTimeAttachment, delayFeedbackAttachment, delayMixAttachment;
    std::unique_ptr<ButtonAttachment> reverbOnAttachment;
    std::unique_ptr<SliderAttachment> reverbRoomAttachment, reverbDampingAttachment, reverbMixAttachment;
    std::unique_ptr<ButtonAttachment> chorusOnAttachment;
    std::unique_ptr<SliderAttachment> chorusRateAttachment, chorusDepthAttachment, chorusMixAttachment;
    std::unique_ptr<ButtonAttachment> distortionOnAttachment;
    std::unique_ptr<SliderAttachment> distortionDriveAttachment, distortionToneAttachment, distortionMixAttachment;

    juce::AudioProcessorValueTreeState& apvts;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectsRackModule)
};
