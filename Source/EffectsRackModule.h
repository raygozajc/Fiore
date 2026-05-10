#pragma once

#include <array>
#include <JuceHeader.h>

/**
    A right-side rack for up to four insert effects.
*/
class EffectsRackModule: public juce::Component,
                         private juce::AudioProcessorValueTreeState::Listener {
public:
    EffectsRackModule(juce::AudioProcessorValueTreeState& apvts);
    ~EffectsRackModule() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    static constexpr int numSlots = 4;

    enum EffectChoice {
        noEffect = 0,
        delayEffect = 1,
        reverbEffect = 2,
        chorusEffect = 3,
        distortionEffect = 4,
        phaserEffect = 5
    };

    void configureSlider(juce::Slider& slider,
                         juce::Label& label,
                         juce::Label& valueLabel,
                         const juce::String& labelText,
                         const juce::String& textValueSuffix,
                         int numDecimalPlacesToDisplay,
                         double defaultValue,
                         const juce::String& paramID,
                         std::unique_ptr<SliderAttachment>& attachmentToCreate);

    void parameterChanged(const juce::String& parameterID, float newValue) override;
    void configureSlotControls();
    void updateSlotMenus();
    void updateEffectVisibility();
    void setSlotSelection(int slotIndex, int effectChoice);
    int getSlotSelection(int slotIndex) const;
    bool isEffectSelectedInAnotherSlot(int slotIndex, int effectChoice) const;
    juce::String getSlotParamID(int slotIndex) const;

    juce::Label moduleLabel;
    std::array<juce::Label, numSlots> slotNumberLabels;
    std::array<juce::ComboBox, numSlots> slotMenus;
    std::array<juce::TextButton, numSlots> removeSlotButtons;

    juce::Label delaySlotLabel;
    juce::TextButton delayOnButton {"On"};
    juce::Label reverbSlotLabel;
    juce::TextButton reverbOnButton {"On"};
    juce::Label chorusSlotLabel;
    juce::TextButton chorusOnButton {"On"};
    juce::Label distortionSlotLabel;
    juce::TextButton distortionOnButton {"Off"};
    juce::Label phaserSlotLabel;
    juce::TextButton phaserOnButton {"Off"};

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

    juce::Label phaserRateLabel, phaserDepthLabel, phaserMixLabel;
    juce::Label phaserRateValueLabel, phaserDepthValueLabel, phaserMixValueLabel;
    juce::Slider phaserRateSlider, phaserDepthSlider, phaserMixSlider;

    std::unique_ptr<ButtonAttachment> delayOnAttachment;
    std::unique_ptr<SliderAttachment> delayTimeAttachment, delayFeedbackAttachment, delayMixAttachment;
    std::unique_ptr<ButtonAttachment> reverbOnAttachment;
    std::unique_ptr<SliderAttachment> reverbRoomAttachment, reverbDampingAttachment, reverbMixAttachment;
    std::unique_ptr<ButtonAttachment> chorusOnAttachment;
    std::unique_ptr<SliderAttachment> chorusRateAttachment, chorusDepthAttachment, chorusMixAttachment;
    std::unique_ptr<ButtonAttachment> distortionOnAttachment;
    std::unique_ptr<SliderAttachment> distortionDriveAttachment, distortionToneAttachment, distortionMixAttachment;
    std::unique_ptr<ButtonAttachment> phaserOnAttachment;
    std::unique_ptr<SliderAttachment> phaserRateAttachment, phaserDepthAttachment, phaserMixAttachment;

    juce::AudioProcessorValueTreeState& apvts;
    bool updatingSlotMenus = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectsRackModule)
};
