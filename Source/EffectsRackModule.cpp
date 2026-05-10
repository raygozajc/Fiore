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
    configureSlider(delayFeedbackSlider, delayFeedbackLabel, delayFeedbackValueLabel, "Feedback", "%", 0, 35.0, "DELAY_FEEDBACK", delayFeedbackAttachment);
    configureSlider(delayMixSlider, delayMixLabel, delayMixValueLabel, "Mix", "%", 0, 25.0, "DELAY_MIX", delayMixAttachment);

    auto configureEmptySlot = [this] (juce::Label& label, const juce::String& text) {
        addAndMakeVisible(label);
        label.setText(text, juce::dontSendNotification);
        label.setFont(juce::Font (juce::FontOptions (14.0f, juce::Font::bold)));
        label.setJustificationType(juce::Justification::centred);
    };

    configureEmptySlot(emptySlot2, "2  EMPTY");
    configureEmptySlot(emptySlot3, "3  EMPTY");
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

    auto delaySlot = slotArea.removeFromTop(slotHeight).reduced(8, 10);
    delaySlotLabel.setBounds(delaySlot.removeFromTop(22));

    auto onRow = delaySlot.removeFromTop(26);
    delayOnButton.setBounds(onRow.removeFromRight(58));
    delaySlot.removeFromTop(4);

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

    constexpr int rowHeight = 24;
    constexpr int rowPitch = 30;
    const auto rowsTop = delaySlot.getY();

    auto timeRow = delaySlot.withY(rowsTop).withHeight(rowHeight);
    placeSlider(timeRow, delayTimeLabel, delayTimeSlider, delayTimeValueLabel);

    auto feedbackRow = delaySlot.withY(rowsTop + rowPitch).withHeight(rowHeight);
    placeSlider(feedbackRow, delayFeedbackLabel, delayFeedbackSlider, delayFeedbackValueLabel);

    auto mixRow = delaySlot.withY(rowsTop + (rowPitch * 2)).withHeight(rowHeight);
    placeSlider(mixRow, delayMixLabel, delayMixSlider, delayMixValueLabel);

    emptySlot2.setBounds(slotArea.removeFromTop(slotHeight).reduced(8, 10));
    emptySlot3.setBounds(slotArea.removeFromTop(slotHeight).reduced(8, 10));
    emptySlot4.setBounds(slotArea.removeFromTop(slotHeight).reduced(8, 10));
}
