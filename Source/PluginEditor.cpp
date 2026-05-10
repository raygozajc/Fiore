#include "PluginProcessor.h"
#include "PluginEditor.h"

Editor::Editor (FioreAudioProcessor& p):
        AudioProcessorEditor (&p), audioProcessor (p),
oscModule(p.getAPVTS()), filterModule(p.getAPVTS()), lfoVibratoModule(p.getAPVTS()), envModule(p.getAPVTS()), ampModule(p.getAPVTS()), effectsRackModule(p.getAPVTS())
{
    addAndMakeVisible(oscModule);
    addAndMakeVisible(filterModule);
    addAndMakeVisible(ampModule);
    addAndMakeVisible(lfoVibratoModule);
    addAndMakeVisible(envModule);
    addAndMakeVisible(effectsRackModule);
    
    setSize (1155, 800);
}

Editor::~Editor() {
}

void Editor::resized() {
    auto area = getLocalBounds().reduced(5);

    auto effectsArea = area.removeFromRight(380);
    area.removeFromRight(5);
    effectsRackModule.setBounds(effectsArea);
    
    auto topRow = area.removeFromTop(300);
    area.removeFromTop(5);
    auto bottomRow = area;
    
    oscModule.setBounds(topRow.removeFromLeft(455));
    topRow.removeFromLeft(5);
    filterModule.setBounds(topRow.removeFromLeft(300));
    
    ampModule.setBounds(bottomRow.removeFromLeft(150));
    bottomRow.removeFromLeft(5);
    lfoVibratoModule.setBounds(bottomRow.removeFromLeft(300));
    bottomRow.removeFromLeft(5);
    envModule.setBounds(bottomRow.removeFromLeft(300));
}

void Editor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::whitesmoke);
}
