#include <JuceHeader.h>
#include "EffectsRackModule.h"

namespace {
    std::size_t slotToIndex(int slotIndex) {
        return static_cast<std::size_t>(slotIndex);
    }
}

EffectsRackModule::EffectsRackModule(juce::AudioProcessorValueTreeState& processorState): apvts(processorState) {
    addAndMakeVisible(moduleLabel);
    moduleLabel.setText("INSERT FX", juce::dontSendNotification);
    moduleLabel.setFont(juce::Font (juce::FontOptions (16.0f, juce::Font::bold)));
    moduleLabel.setJustificationType(juce::Justification::centred);

    configureSlotControls();

    addAndMakeVisible(delaySlotLabel);
    delaySlotLabel.setText("DELAY", juce::dontSendNotification);
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
    reverbSlotLabel.setText("REVERB", juce::dontSendNotification);
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
    chorusSlotLabel.setText("CHORUS", juce::dontSendNotification);
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

    addAndMakeVisible(distortionSlotLabel);
    distortionSlotLabel.setText("DIST", juce::dontSendNotification);
    distortionSlotLabel.setFont(juce::Font (juce::FontOptions (14.0f, juce::Font::bold)));
    distortionSlotLabel.setJustificationType(juce::Justification::centredLeft);

    addAndMakeVisible(distortionOnButton);
    distortionOnButton.setToggleable(true);
    distortionOnButton.setClickingTogglesState(true);
    distortionOnButton.setToggleState(false, juce::dontSendNotification);
    distortionOnButton.onClick = [this] {
        distortionOnButton.setButtonText(distortionOnButton.getToggleState() ? "On" : "Off");
    };
    distortionOnAttachment = std::make_unique<ButtonAttachment>(apvts, "DIST_ON", distortionOnButton);

    configureSlider(distortionDriveSlider, distortionDriveLabel, distortionDriveValueLabel, "Drive", "%", 0, 45.0, "DIST_DRIVE", distortionDriveAttachment);
    configureSlider(distortionToneSlider, distortionToneLabel, distortionToneValueLabel, "Tone", "%", 0, 65.0, "DIST_TONE", distortionToneAttachment);
    configureSlider(distortionMixSlider, distortionMixLabel, distortionMixValueLabel, "Mix", "%", 0, 45.0, "DIST_MIX", distortionMixAttachment);

    addAndMakeVisible(phaserSlotLabel);
    phaserSlotLabel.setText("PHASER", juce::dontSendNotification);
    phaserSlotLabel.setFont(juce::Font (juce::FontOptions (14.0f, juce::Font::bold)));
    phaserSlotLabel.setJustificationType(juce::Justification::centredLeft);

    addAndMakeVisible(phaserOnButton);
    phaserOnButton.setToggleable(true);
    phaserOnButton.setClickingTogglesState(true);
    phaserOnButton.setToggleState(false, juce::dontSendNotification);
    phaserOnButton.onClick = [this] {
        phaserOnButton.setButtonText(phaserOnButton.getToggleState() ? "On" : "Off");
    };
    phaserOnAttachment = std::make_unique<ButtonAttachment>(apvts, "PHASER_ON", phaserOnButton);

    configureSlider(phaserRateSlider, phaserRateLabel, phaserRateValueLabel, "Rate", " Hz", 1, 0.7, "PHASER_RATE", phaserRateAttachment);
    configureSlider(phaserDepthSlider, phaserDepthLabel, phaserDepthValueLabel, "Depth", "%", 0, 35.0, "PHASER_DEPTH", phaserDepthAttachment);
    configureSlider(phaserMixSlider, phaserMixLabel, phaserMixValueLabel, "Mix", "%", 0, 22.0, "PHASER_MIX", phaserMixAttachment);

    for (int slot = 0; slot < numSlots; ++slot)
        apvts.addParameterListener(getSlotParamID(slot), this);

    updateSlotMenus();
    updateEffectVisibility();
}

EffectsRackModule::~EffectsRackModule() {
    for (int slot = 0; slot < numSlots; ++slot)
        apvts.removeParameterListener(getSlotParamID(slot), this);
}

void EffectsRackModule::configureSlotControls() {
    for (int slot = 0; slot < numSlots; ++slot) {
        auto& slotLabel = slotNumberLabels[slotToIndex(slot)];
        addAndMakeVisible(slotLabel);
        slotLabel.setText(juce::String(slot + 1), juce::dontSendNotification);
        slotLabel.setFont(juce::Font(juce::FontOptions(14.0f, juce::Font::bold)));
        slotLabel.setJustificationType(juce::Justification::centred);

        auto& menu = slotMenus[slotToIndex(slot)];
        addAndMakeVisible(menu);
        menu.setJustificationType(juce::Justification::centredLeft);
        menu.onChange = [this, slot] {
            if (updatingSlotMenus)
                return;

            const auto selectedEffect = slotMenus[slotToIndex(slot)].getSelectedId() - 1;
            setSlotSelection(slot, juce::jmax(static_cast<int>(noEffect), selectedEffect));
        };

        auto& removeButton = removeSlotButtons[slotToIndex(slot)];
        addAndMakeVisible(removeButton);
        removeButton.setButtonText("X");
        removeButton.onClick = [this, slot] {
            setSlotSelection(slot, noEffect);
        };
    }
}

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

void EffectsRackModule::parameterChanged(const juce::String& parameterID, float newValue) {
    juce::ignoreUnused(parameterID, newValue);

    juce::Component::SafePointer<EffectsRackModule> safeThis(this);
    juce::MessageManager::callAsync([safeThis] {
        if (safeThis == nullptr)
            return;

        safeThis->updateSlotMenus();
        safeThis->updateEffectVisibility();
        safeThis->resized();
        safeThis->repaint();
    });
}

juce::String EffectsRackModule::getSlotParamID(int slotIndex) const {
    return "INSERT_SLOT_" + juce::String(slotIndex + 1);
}

int EffectsRackModule::getSlotSelection(int slotIndex) const {
    if (auto* value = apvts.getRawParameterValue(getSlotParamID(slotIndex)))
        return juce::roundToInt(value->load());

    return noEffect;
}

void EffectsRackModule::setSlotSelection(int slotIndex, int effectChoice) {
    if (auto* parameter = apvts.getParameter(getSlotParamID(slotIndex))) {
        parameter->beginChangeGesture();
        parameter->setValueNotifyingHost(parameter->convertTo0to1(static_cast<float>(effectChoice)));
        parameter->endChangeGesture();
    }
    
    // When an effect is added, turn it ON by default (except for "None")
    if (effectChoice != noEffect) {
        juce::String effectOnParamID;
        switch (effectChoice) {
            case delayEffect:
                effectOnParamID = "DELAY_ON";
                break;
            case reverbEffect:
                effectOnParamID = "REVERB_ON";
                break;
            case chorusEffect:
                effectOnParamID = "CHORUS_ON";
                break;
            case distortionEffect:
                effectOnParamID = "DIST_ON";
                break;
            case phaserEffect:
                effectOnParamID = "PHASER_ON";
                break;
            default:
                break;
        }
        
        if (!effectOnParamID.isEmpty()) {
            if (auto* onOffParameter = apvts.getParameter(effectOnParamID)) {
                onOffParameter->beginChangeGesture();
                onOffParameter->setValueNotifyingHost(1.0f); // Turn ON
                onOffParameter->endChangeGesture();
            }
        }
    }
}

bool EffectsRackModule::isEffectSelectedInAnotherSlot(int slotIndex, int effectChoice) const {
    if (effectChoice == noEffect)
        return false;

    for (int otherSlot = 0; otherSlot < numSlots; ++otherSlot) {
        if (otherSlot != slotIndex && getSlotSelection(otherSlot) == effectChoice)
            return true;
    }

    return false;
}

void EffectsRackModule::updateSlotMenus() {
    static const juce::StringArray effectNames { "None", "Delay", "Reverb", "Chorus", "Dist", "Phaser" };

    updatingSlotMenus = true;

    for (int slot = 0; slot < numSlots; ++slot) {
        auto& menu = slotMenus[slotToIndex(slot)];
        const auto selectedEffect = getSlotSelection(slot);

        menu.clear(juce::dontSendNotification);
        menu.addItem(effectNames[noEffect], noEffect + 1);

        for (int effect = delayEffect; effect <= phaserEffect; ++effect) {
            if (effect == selectedEffect || ! isEffectSelectedInAnotherSlot(slot, effect))
                menu.addItem(effectNames[effect], effect + 1);
        }

        menu.setSelectedId(selectedEffect + 1, juce::dontSendNotification);
        removeSlotButtons[slotToIndex(slot)].setEnabled(selectedEffect != noEffect);
    }

    updatingSlotMenus = false;
}

void EffectsRackModule::updateEffectVisibility() {
    const auto showDelay = [this] (bool shouldShow) {
        delaySlotLabel.setVisible(shouldShow);
        delayOnButton.setVisible(shouldShow);
        delayTimeLabel.setVisible(shouldShow);
        delayTimeSlider.setVisible(shouldShow);
        delayTimeValueLabel.setVisible(shouldShow);
        delayFeedbackLabel.setVisible(shouldShow);
        delayFeedbackSlider.setVisible(shouldShow);
        delayFeedbackValueLabel.setVisible(shouldShow);
        delayMixLabel.setVisible(shouldShow);
        delayMixSlider.setVisible(shouldShow);
        delayMixValueLabel.setVisible(shouldShow);
    };

    const auto showReverb = [this] (bool shouldShow) {
        reverbSlotLabel.setVisible(shouldShow);
        reverbOnButton.setVisible(shouldShow);
        reverbRoomLabel.setVisible(shouldShow);
        reverbRoomSlider.setVisible(shouldShow);
        reverbRoomValueLabel.setVisible(shouldShow);
        reverbDampingLabel.setVisible(shouldShow);
        reverbDampingSlider.setVisible(shouldShow);
        reverbDampingValueLabel.setVisible(shouldShow);
        reverbMixLabel.setVisible(shouldShow);
        reverbMixSlider.setVisible(shouldShow);
        reverbMixValueLabel.setVisible(shouldShow);
    };

    const auto showChorus = [this] (bool shouldShow) {
        chorusSlotLabel.setVisible(shouldShow);
        chorusOnButton.setVisible(shouldShow);
        chorusRateLabel.setVisible(shouldShow);
        chorusRateSlider.setVisible(shouldShow);
        chorusRateValueLabel.setVisible(shouldShow);
        chorusDepthLabel.setVisible(shouldShow);
        chorusDepthSlider.setVisible(shouldShow);
        chorusDepthValueLabel.setVisible(shouldShow);
        chorusMixLabel.setVisible(shouldShow);
        chorusMixSlider.setVisible(shouldShow);
        chorusMixValueLabel.setVisible(shouldShow);
    };

    const auto showDistortion = [this] (bool shouldShow) {
        distortionSlotLabel.setVisible(shouldShow);
        distortionOnButton.setVisible(shouldShow);
        distortionDriveLabel.setVisible(shouldShow);
        distortionDriveSlider.setVisible(shouldShow);
        distortionDriveValueLabel.setVisible(shouldShow);
        distortionToneLabel.setVisible(shouldShow);
        distortionToneSlider.setVisible(shouldShow);
        distortionToneValueLabel.setVisible(shouldShow);
        distortionMixLabel.setVisible(shouldShow);
        distortionMixSlider.setVisible(shouldShow);
        distortionMixValueLabel.setVisible(shouldShow);
    };

    const auto showPhaser = [this] (bool shouldShow) {
        phaserSlotLabel.setVisible(shouldShow);
        phaserOnButton.setVisible(shouldShow);
        phaserRateLabel.setVisible(shouldShow);
        phaserRateSlider.setVisible(shouldShow);
        phaserRateValueLabel.setVisible(shouldShow);
        phaserDepthLabel.setVisible(shouldShow);
        phaserDepthSlider.setVisible(shouldShow);
        phaserDepthValueLabel.setVisible(shouldShow);
        phaserMixLabel.setVisible(shouldShow);
        phaserMixSlider.setVisible(shouldShow);
        phaserMixValueLabel.setVisible(shouldShow);
    };

    showDelay(false);
    showReverb(false);
    showChorus(false);
    showDistortion(false);
    showPhaser(false);

    for (int slot = 0; slot < numSlots; ++slot) {
        switch (getSlotSelection(slot)) {
            case delayEffect:      showDelay(true); break;
            case reverbEffect:     showReverb(true); break;
            case chorusEffect:     showChorus(true); break;
            case distortionEffect: showDistortion(true); break;
            case phaserEffect:     showPhaser(true); break;
            default: break;
        }
    }
}

void EffectsRackModule::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::slategrey.darker(0.25f));

    auto area = getLocalBounds();
    area.removeFromBottom(area.getHeight() / 12);
    area.removeFromTop(8);

    g.setColour(juce::Colours::whitesmoke);
    for (int i = 0; i < 4; ++i) {
        auto slot = area.removeFromTop(area.getHeight() / (4 - i)).reduced(8, 4);
        g.drawRect(slot, 1);
    }
}

void EffectsRackModule::resized() {
    auto area = getLocalBounds();
    moduleLabel.setBounds(area.removeFromBottom(area.getHeight() / 12));

    area.removeFromTop(8);
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
                                           juce::Label& slotNumberLabel,
                                           juce::ComboBox& slotMenu,
                                           juce::TextButton& removeButton,
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
        auto content = slot.reduced(8, 8);
        auto header = content.removeFromTop(26);
        slotNumberLabel.setBounds(header.removeFromLeft(24));
        header.removeFromLeft(6);
        removeButton.setBounds(header.removeFromRight(24).reduced(0, 1));
        header.removeFromRight(6);
        onButton.setBounds(header.removeFromRight(52).reduced(0, 1));
        header.removeFromRight(6);
        slotMenu.setBounds(header.reduced(0, 1));

        auto titleRow = content.removeFromTop(20);
        title.setBounds(titleRow.removeFromLeft(100));

        content.removeFromTop(1);

        constexpr int rowHeight = 24;
        constexpr int rowPitch = 28;
        const auto rowsTop = content.getY();

        placeSlider(content.withY(rowsTop).withHeight(rowHeight), row1Label, row1Slider, row1ValueLabel);
        placeSlider(content.withY(rowsTop + rowPitch).withHeight(rowHeight), row2Label, row2Slider, row2ValueLabel);
        placeSlider(content.withY(rowsTop + (rowPitch * 2)).withHeight(rowHeight), row3Label, row3Slider, row3ValueLabel);
    };

    auto placeEmptySlot = [] (juce::Rectangle<int> slot,
                              juce::Label& slotNumberLabel,
                              juce::ComboBox& slotMenu,
                              juce::TextButton& removeButton) {
        auto content = slot.reduced(8, 8);
        auto header = content.removeFromTop(26);
        slotNumberLabel.setBounds(header.removeFromLeft(24));
        header.removeFromLeft(6);
        removeButton.setBounds(header.removeFromRight(24).reduced(0, 1));
        header.removeFromRight(64);
        slotMenu.setBounds(header.reduced(0, 1));
    };

    std::array<juce::Rectangle<int>, numSlots> slots;
    for (int slot = 0; slot < numSlots; ++slot)
        slots[slotToIndex(slot)] = slotArea.removeFromTop(slotHeight);

    for (int slot = 0; slot < numSlots; ++slot)
        placeEmptySlot(slots[slotToIndex(slot)],
                       slotNumberLabels[slotToIndex(slot)],
                       slotMenus[slotToIndex(slot)],
                       removeSlotButtons[slotToIndex(slot)]);

    auto selectedSlotForEffect = [this] (int effectChoice) {
        for (int slot = 0; slot < numSlots; ++slot)
            if (getSlotSelection(slot) == effectChoice)
                return slot;

        return -1;
    };

    if (const auto slot = selectedSlotForEffect(delayEffect); slot >= 0)
        placeEffectSlot(slots[slotToIndex(slot)],
                    slotNumberLabels[slotToIndex(slot)],
                    slotMenus[slotToIndex(slot)],
                    removeSlotButtons[slotToIndex(slot)],
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

    if (const auto slot = selectedSlotForEffect(reverbEffect); slot >= 0)
        placeEffectSlot(slots[slotToIndex(slot)],
                    slotNumberLabels[slotToIndex(slot)],
                    slotMenus[slotToIndex(slot)],
                    removeSlotButtons[slotToIndex(slot)],
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

    if (const auto slot = selectedSlotForEffect(chorusEffect); slot >= 0)
        placeEffectSlot(slots[slotToIndex(slot)],
                    slotNumberLabels[slotToIndex(slot)],
                    slotMenus[slotToIndex(slot)],
                    removeSlotButtons[slotToIndex(slot)],
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

    if (const auto slot = selectedSlotForEffect(distortionEffect); slot >= 0)
        placeEffectSlot(slots[slotToIndex(slot)],
                    slotNumberLabels[slotToIndex(slot)],
                    slotMenus[slotToIndex(slot)],
                    removeSlotButtons[slotToIndex(slot)],
                    distortionSlotLabel,
                    distortionOnButton,
                    distortionDriveLabel,
                    distortionDriveSlider,
                    distortionDriveValueLabel,
                    distortionToneLabel,
                    distortionToneSlider,
                    distortionToneValueLabel,
                    distortionMixLabel,
                    distortionMixSlider,
                    distortionMixValueLabel);

    if (const auto slot = selectedSlotForEffect(phaserEffect); slot >= 0)
        placeEffectSlot(slots[slotToIndex(slot)],
                    slotNumberLabels[slotToIndex(slot)],
                    slotMenus[slotToIndex(slot)],
                    removeSlotButtons[slotToIndex(slot)],
                    phaserSlotLabel,
                    phaserOnButton,
                    phaserRateLabel,
                    phaserRateSlider,
                    phaserRateValueLabel,
                    phaserDepthLabel,
                    phaserDepthSlider,
                    phaserDepthValueLabel,
                    phaserMixLabel,
                    phaserMixSlider,
                    phaserMixValueLabel);
}
