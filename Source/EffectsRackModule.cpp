#include <JuceHeader.h>
#include "EffectsRackModule.h"

EffectsRackModule::EffectsRackModule(juce::AudioProcessorValueTreeState& processorState): apvts(processorState) {
    addAndMakeVisible(moduleLabel);
    moduleLabel.setText("INSERT FX", juce::dontSendNotification);
    moduleLabel.setFont(juce::Font (juce::FontOptions (16.0f, juce::Font::bold)));
    moduleLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(delaySlotLabel);
    delaySlotLabel.setText("1  DELAY", juce::dontSendNotification);
    delaySlotLabel.setFont(juce::Font (juce::FontOptions (14.0f, juce::Font::bold)));
    delaySlotLabel.setJustificationType(juce::Justification::centredLeft);

    addAndMakeVisible(delayOnButton);
    delayOnButton.setToggleable(true);
    delayOnButton.setClickingTogglesState(true);
    delayOnButton.setToggleState(true, juce::dontSendNotification);
    delayOnButton.onClick = [this] {
        delayOnButton.setButtonText(delayOnButton.getToggleState() ? "On" : "Off");
    };
    delayOnAttachment = std::make_unique<ButtonAttachment>(apvts, "DELAY_ON", delayOnButton);

    configureSlider(delayTimeSlider, delayTimeLabel, delayTimeValueLabel, "Time", " ms", 0, 350.0, "DELAY_TIME", delayTimeAttachment);
    configureSlider(delayFeedbackSlider, delayFeedbackLabel, delayFeedbackValueLabel, "Feedback", "%", 0, 30.0, "DELAY_FEEDBACK", delayFeedbackAttachment);
    configureSlider(delayMixSlider, delayMixLabel, delayMixValueLabel, "Mix", "%", 0, 25.0, "DELAY_MIX", delayMixAttachment);

    addAndMakeVisible(reverbSlotLabel);
    reverbSlotLabel.setText("2  REVERB", juce::dontSendNotification);
    reverbSlotLabel.setFont(juce::Font (juce::FontOptions (14.0f, juce::Font::bold)));
    reverbSlotLabel.setJustificationType(juce::Justification::centredLeft);

    addAndMakeVisible(reverbOnButton);
    reverbOnButton.setToggleable(true);
    reverbOnButton.setClickingTogglesState(true);
    reverbOnButton.setToggleState(true, juce::dontSendNotification);
    reverbOnButton.onClick = [this] {
        reverbOnButton.setButtonText(reverbOnButton.getToggleState() ? "On" : "Off");
    };
    reverbOnAttachment = std::make_unique<ButtonAttachment>(apvts, "REVERB_ON", reverbOnButton);

    configureSlider(reverbRoomSlider, reverbRoomLabel, reverbRoomValueLabel, "Room", "%", 0, 40.0, "REVERB_ROOM", reverbRoomAttachment);
    configureSlider(reverbDampingSlider, reverbDampingLabel, reverbDampingValueLabel, "Damp", "%", 0, 50.0, "REVERB_DAMPING", reverbDampingAttachment);
    configureSlider(reverbMixSlider, reverbMixLabel, reverbMixValueLabel, "Mix", "%", 0, 18.0, "REVERB_MIX", reverbMixAttachment);

    addAndMakeVisible(chorusSlotLabel);
    chorusSlotLabel.setText("3  CHORUS", juce::dontSendNotification);
    chorusSlotLabel.setFont(juce::Font (juce::FontOptions (14.0f, juce::Font::bold)));
    chorusSlotLabel.setJustificationType(juce::Justification::centredLeft);

    addAndMakeVisible(chorusOnButton);
    chorusOnButton.setToggleable(true);
    chorusOnButton.setClickingTogglesState(true);
    chorusOnButton.setToggleState(true, juce::dontSendNotification);
    chorusOnButton.onClick = [this] {
        chorusOnButton.setButtonText(chorusOnButton.getToggleState() ? "On" : "Off");
    };
    chorusOnAttachment = std::make_unique<ButtonAttachment>(apvts, "CHORUS_ON", chorusOnButton);

    configureSlider(chorusRateSlider, chorusRateLabel, chorusRateValueLabel, "Rate", " Hz", 1, 0.7, "CHORUS_RATE", chorusRateAttachment);
    configureSlider(chorusDepthSlider, chorusDepthLabel, chorusDepthValueLabel, "Depth", "%", 0, 25.0, "CHORUS_DEPTH", chorusDepthAttachment);
    configureSlider(chorusMixSlider, chorusMixLabel, chorusMixValueLabel, "Mix", "%", 0, 18.0, "CHORUS_MIX", chorusMixAttachment);

    auto configureEmptySlot = [this] (juce::Label& label, const juce::String& text) {
        addAndMakeVisible(label);
        label.setText(text, juce::dontSendNotification);
        label.setFont(juce::Font (juce::FontOptions (14.0f, juce::Font::bold)));
        label.setJustificationType(juce::Justification::centred);
    };

    configureEmptySlot(emptySlot4, "4  EMPTY");
}

EffectsRackModule::~EffectsRackModule() {}

void EffectsRackModule::configureSlider(juce::Slider& slider,
                                        juce::Label& label,
                                        juce::Label& valueLabel,
                                        const juce::String& labelText,
                                        const juce::String& textValueSuffix,
                                        int numDecimalPlacesToDisplay,
                                        double defaultValue,
                                        const juce::String& paramID,
                                        std::unique_ptr<SliderAttachment>& attachmentToCreate) {
    addAndMakeVisible(slider);
    slider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    slider.setDoubleClickReturnValue(true, defaultValue);
    slider.setNumDecimalPlacesToDisplay(numDecimalPlacesToDisplay);
    slider.setTextValueSuffix(textValueSuffix);

    addAndMakeVisible(label);
    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centredLeft);

    attachmentToCreate = std::make_unique<SliderAttachment>(apvts, paramID, slider);

    addAndMakeVisible(valueLabel);
    valueLabel.setJustificationType(juce::Justification::centred);
    valueLabel.setColour(juce::Label::outlineColourId, juce::Colours::whitesmoke.withAlpha(0.55f));
    valueLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);

    auto updateValueLabel = [&slider, &valueLabel, textValueSuffix, numDecimalPlacesToDisplay] {
        valueLabel.setText(juce::String(slider.getValue(), numDecimalPlacesToDisplay) + textValueSuffix,
                           juce::dontSendNotification);
    };
    slider.onValueChange = updateValueLabel;
    updateValueLabel();
}

void EffectsRackModule::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::slategrey.darker(0.25f));

    auto area = getLocalBounds();
    area.removeFromTop(34);
    area.removeFromBottom(area.getHeight() / 12);

    g.setColour(juce::Colours::whitesmoke);
    for (int i = 0; i < 4; ++i) {
        auto slot = area.removeFromTop(area.getHeight() / (4 - i)).reduced(8, 5);
        g.drawRect(slot, 1);
    }
}

void EffectsRackModule::resized() {
    auto area = getLocalBounds();
    moduleLabel.setBounds(area.removeFromBottom(area.getHeight() / 12));

    area.removeFromTop(10);
    auto slotArea = area.reduced(8, 0);
    const auto slotHeight = slotArea.getHeight() / 4;

    auto placeSlider = [] (juce::Rectangle<int> row, juce::Label& label, juce::Slider& slider, juce::Label& valueLabel) {
        constexpr int labelWidth = 92;
        constexpr int sliderTrackWidth = 155;
        constexpr int sliderValueGap = 10;
        constexpr int valueBoxWidth = 82;

        auto labelArea = row.removeFromLeft(labelWidth);
        label.setBounds(labelArea.withSizeKeepingCentre(labelArea.getWidth(), 24));

        auto sliderArea = row.removeFromLeft(sliderTrackWidth);
        slider.setBounds(sliderArea.withSizeKeepingCentre(sliderArea.getWidth(), 24));

        row.removeFromLeft(sliderValueGap);
        auto valueArea = row.removeFromLeft(valueBoxWidth);
        valueLabel.setBounds(valueArea.withSizeKeepingCentre(valueArea.getWidth(), 24));
    };

    auto placeEffectSlot = [&placeSlider] (juce::Rectangle<int> slot,
                                           juce::Label& title,
                                           juce::TextButton& onButton,
                                           juce::Label& row1Label,
                                           juce::Slider& row1Slider,
                                           juce::Label& row1ValueLabel,
                                           juce::Label& row2Label,
                                           juce::Slider& row2Slider,
                                           juce::Label& row2ValueLabel,
                                           juce::Label& row3Label,
                                           juce::Slider& row3Slider,
                                           juce::Label& row3ValueLabel) {
        auto content = slot.reduced(8, 10);
        title.setBounds(content.removeFromTop(22));

        auto onRow = content.removeFromTop(26);
        onButton.setBounds(onRow.removeFromRight(58));
        content.removeFromTop(4);

        constexpr int rowHeight = 24;
        constexpr int rowPitch = 30;
        const auto rowsTop = content.getY();

        placeSlider(content.withY(rowsTop).withHeight(rowHeight), row1Label, row1Slider, row1ValueLabel);
        placeSlider(content.withY(rowsTop + rowPitch).withHeight(rowHeight), row2Label, row2Slider, row2ValueLabel);
        placeSlider(content.withY(rowsTop + (rowPitch * 2)).withHeight(rowHeight), row3Label, row3Slider, row3ValueLabel);
    };

    placeEffectSlot(slotArea.removeFromTop(slotHeight),
                    delaySlotLabel,
                    delayOnButton,
                    delayTimeLabel,
                    delayTimeSlider,
                    delayTimeValueLabel,
                    delayFeedbackLabel,
                    delayFeedbackSlider,
                    delayFeedbackValueLabel,
                    delayMixLabel,
                    delayMixSlider,
                    delayMixValueLabel);

    placeEffectSlot(slotArea.removeFromTop(slotHeight),
                    reverbSlotLabel,
                    reverbOnButton,
                    reverbRoomLabel,
                    reverbRoomSlider,
                    reverbRoomValueLabel,
                    reverbDampingLabel,
                    reverbDampingSlider,
                    reverbDampingValueLabel,
                    reverbMixLabel,
                    reverbMixSlider,
                    reverbMixValueLabel);

    placeEffectSlot(slotArea.removeFromTop(slotHeight),
                    chorusSlotLabel,
                    chorusOnButton,
                    chorusRateLabel,
                    chorusRateSlider,
                    chorusRateValueLabel,
                    chorusDepthLabel,
                    chorusDepthSlider,
                    chorusDepthValueLabel,
                    chorusMixLabel,
                    chorusMixSlider,
                    chorusMixValueLabel);

    emptySlot4.setBounds(slotArea.removeFromTop(slotHeight).reduced(8, 10));
}
