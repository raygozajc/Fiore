#include "PluginProcessor.h"
#include "PluginEditor.h"

FioreAudioProcessor::FioreAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParams())
#endif
{
    for (int i = 0; i < 16; i++) {
        synth.addVoice(new SynthVoice());
    }
    synth.addSound(new SynthSound());
    synth.setNoteStealingEnabled(true);
}

FioreAudioProcessor::~FioreAudioProcessor() {}

void FioreAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {
    synth.setCurrentPlaybackSampleRate(sampleRate);
    outputDelay.prepare(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
    outputReverb.prepare(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
    outputChorus.prepare(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
    outputDistortion.prepare(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
    
    for (int i = 0; i < synth.getNumVoices(); i++) {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
            voice->prepareToPlay (sampleRate, samplesPerBlock, getTotalNumOutputChannels());
        }
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout FioreAudioProcessor::createParams() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    // Oscillator Module Params
    params.push_back(std::make_unique<juce::AudioParameterInt>(ParameterID("OSC1_WAVE", 1), "Oscillator 1 Waveform", 0, 3, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(ParameterID("OSC2_WAVE", 1), "Oscillator 2 Waveform", 0, 3, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("OSC1_GAIN_RATIO", 1), "Oscillator 1 Gain Ratio", 0.0, 1.0, 1.0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(ParameterID("DETUNE_CENTS_1", 1), "Osc. 1 Detune (Cents)", -50, 50, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(ParameterID("DETUNE_CENTS", 1), "Osc. 2 Detune (Cents)", -50, 50, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(ParameterID("DETUNE_SEMI", 1), "Osc. 2 Detune (Semitones)", -24, 24, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(ParameterID("SINE_LVL", 1), "Sine Osc. Level", 0, 100, 0));
    
    // Filter Module Params
    params.push_back(std::make_unique<juce::AudioParameterChoice>(ParameterID("FILT_TYPE", 1), "Filter Type", juce::StringArray {"LPF12", "HPF12", "BPF12", "LPF24", "HPF24", "BPF24"}, 0));
    juce::NormalisableRange<float> cutoffRange {20.0, 10000.0, 1.0};
    cutoffRange.setSkewForCentre(500.0);
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("FILT_CUTOFF", 1), "Cutoff", cutoffRange, 1000.0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(ParameterID("FILT_RESO", 1), "Resonance", 0, 100, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(ParameterID("FILT_DRIVE_AMT", 1), "Filter Drive Amount", 0, 100, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(ParameterID("FILT_ENV_AMT", 1), "Filter Envelope Amount", 0, 100, 0));
    params.push_back(std::make_unique<juce::AudioParameterBool>(ParameterID("FILT_ON_OFF", 1), "Filter On/Off", true));
    
    // Amp Module Params
    juce::NormalisableRange<float> gainRange {-84.0f, 12.0f, 0.1f};
    gainRange.setSkewForCentre(-9.0f);
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("GAIN", 1), "Global Gain", gainRange, 0.0));

    // Insert FX Params
    juce::NormalisableRange<float> delayTimeRange {20.0f, DelayEffect::maxDelayTimeSeconds * 1000.0f, 1.0f};
    delayTimeRange.setSkewForCentre(350.0f);
    params.push_back(std::make_unique<juce::AudioParameterBool>(ParameterID("DELAY_ON", 1), "Delay On/Off", true));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("DELAY_TIME", 1), "Delay Time", delayTimeRange, 350.0f));
    juce::NormalisableRange<float> delayFeedbackRange {0.0f, 75.0f, 1.0f};
    juce::NormalisableRange<float> delayMixRange {0.0f, 60.0f, 1.0f};
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("DELAY_FEEDBACK", 1), "Delay Feedback", delayFeedbackRange, 30.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("DELAY_MIX", 1), "Delay Mix", delayMixRange, 25.0f));

    juce::NormalisableRange<float> reverbRoomRange {5.0f, 90.0f, 1.0f};
    juce::NormalisableRange<float> reverbDampingRange {0.0f, 90.0f, 1.0f};
    juce::NormalisableRange<float> reverbMixRange {0.0f, 50.0f, 1.0f};
    params.push_back(std::make_unique<juce::AudioParameterBool>(ParameterID("REVERB_ON", 1), "Reverb On/Off", true));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("REVERB_ROOM", 1), "Reverb Room Size", reverbRoomRange, 40.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("REVERB_DAMPING", 1), "Reverb Damping", reverbDampingRange, 50.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("REVERB_MIX", 1), "Reverb Mix", reverbMixRange, 18.0f));

    juce::NormalisableRange<float> chorusRateRange {0.1f, 3.0f, 0.1f};
    chorusRateRange.setSkewForCentre(0.7f);
    juce::NormalisableRange<float> chorusDepthRange {0.0f, 100.0f, 1.0f};
    juce::NormalisableRange<float> chorusMixRange {0.0f, 45.0f, 1.0f};
    params.push_back(std::make_unique<juce::AudioParameterBool>(ParameterID("CHORUS_ON", 1), "Chorus On/Off", true));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("CHORUS_RATE", 1), "Chorus Rate", chorusRateRange, 0.7f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("CHORUS_DEPTH", 1), "Chorus Depth", chorusDepthRange, 25.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("CHORUS_MIX", 1), "Chorus Mix", chorusMixRange, 18.0f));

    juce::NormalisableRange<float> distortionPercentRange {0.0f, 100.0f, 1.0f};
    juce::NormalisableRange<float> distortionMixRange {0.0f, 80.0f, 1.0f};
    params.push_back(std::make_unique<juce::AudioParameterBool>(ParameterID("DIST_ON", 1), "Distortion On/Off", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("DIST_DRIVE", 1), "Distortion Drive", distortionPercentRange, 45.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("DIST_TONE", 1), "Distortion Tone", distortionPercentRange, 65.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("DIST_MIX", 1), "Distortion Mix", distortionMixRange, 45.0f));
    
    // LFO/Vibrato Module Params
    juce::NormalisableRange<float> rateRange {0.01f, 200.0f, 0.01f};
    rateRange.setSkewForCentre(15.0f);
    juce::StringArray lfoShapeOptions { "Saw Up", "Saw Down", "Tri", "Square"};
    params.push_back(std::make_unique<juce::AudioParameterChoice>(ParameterID("LFO_SHAPE", 1), "Filter LFO Shape", lfoShapeOptions, 2));
    params.push_back(std::make_unique<juce::AudioParameterInt>(ParameterID("LFO_AMOUNT", 1), "Filter LFO Amount", 0, 100, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("LFO_RATE", 1), "Filter LFO Rate", rateRange, 1.0));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(ParameterID("VIB_SHAPE", 1), "Vibrato Shape", lfoShapeOptions, 2));
    params.push_back(std::make_unique<juce::AudioParameterInt>(ParameterID("VIB_AMOUNT", 1), "Vibrato Amount", 0, 100, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("VIB_RATE", 1), "Vibrato Rate", rateRange, 5.0));
    
    // ADSR Module Params
    juce::NormalisableRange<float> attackRange {0.0001f, 10.0f, 0.0001f}; // sec
    juce::NormalisableRange<float> decayRange {0.0001f, 10.0f, 0.0001f}; // sec
    juce::NormalisableRange<float> sustainRange {1, 100, 1}; // %
    juce::NormalisableRange<float> releaseRange {0.001f, 10.0f, 0.001f}; // sec
    attackRange.setSkewForCentre(0.41f);
    decayRange.setSkewForCentre(0.41f);
    releaseRange.setSkewForCentre(0.5f);
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("AMP_ATK", 1), "Amp Attack", attackRange, 0.0004));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("AMP_DEC", 1), "Amp Decay", decayRange, 0.520));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("AMP_SUS", 1), "Amp Sustain", sustainRange, 75));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("AMP_REL", 1), "Amp Release", releaseRange, 0.038));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("FILT_ATK", 1), "Filter Attack", attackRange, 0.0004));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("FILT_DEC", 1), "Filter Decay", decayRange, 0.520));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("FILT_SUS", 1), "Filter Sustain", sustainRange, 75));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParameterID("FILT_REL", 1), "Filter Release", releaseRange, 0.038));
    
    return { params.begin(), params.end() };
}

void FioreAudioProcessor::updateParams() {
    for (int i = 0; i < synth.getNumVoices(); i++) {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
            
            // Oscillator Module Params
            auto& osc1Choice = *apvts.getRawParameterValue("OSC1_WAVE");
            auto& osc2Choice = *apvts.getRawParameterValue("OSC2_WAVE");
            auto& osc1GainRatio = *apvts.getRawParameterValue("OSC1_GAIN_RATIO");
            auto& osc1DetuneCents = *apvts.getRawParameterValue("DETUNE_CENTS_1");
            auto& osc2DetuneCents = *apvts.getRawParameterValue("DETUNE_CENTS");
            auto& osc2DetuneSemi = *apvts.getRawParameterValue("DETUNE_SEMI");
            auto sineLevel = apvts.getRawParameterValue("SINE_LVL")->load() / 100.0f;
            voice->setOscWaveform(juce::roundToInt(osc1Choice.load()), 1);
            voice->setOscWaveform(juce::roundToInt(osc2Choice.load()), 2);
            voice->setOscGainRatios(osc1GainRatio.load());
            voice->setOscDetune(0, juce::roundToInt(osc1DetuneCents.load()), 1);
            voice->setOscDetune(juce::roundToInt(osc2DetuneSemi.load()), juce::roundToInt(osc2DetuneCents.load()), 2);
            voice->setOscSineLevel(sineLevel);
            
            // Filter Module Params
            auto& filtType = *apvts.getRawParameterValue("FILT_TYPE");
            auto& filtCutoff = *apvts.getRawParameterValue("FILT_CUTOFF");
            auto filtReso = apvts.getRawParameterValue("FILT_RESO")->load() / 100.0f;
            auto filtDriveAmt = apvts.getRawParameterValue("FILT_DRIVE_AMT")->load() / 100.0f;
            auto filtEnvAmt = apvts.getRawParameterValue("FILT_ENV_AMT")->load() / 100.0f;
            auto filtOn = apvts.getRawParameterValue("FILT_ON_OFF")->load() > 0.5f;
            voice->setFilterType(juce::roundToInt(filtType.load()));
            voice->setFilterParams(filtCutoff.load(), filtReso, filtDriveAmt, filtEnvAmt);
            voice->setFilterOnOff(filtOn);
            
            // Amp Module Params
            auto& gain = *apvts.getRawParameterValue("GAIN");
            voice->setMasterGain(gain.load());
            
            // LFO/Vibrato Module Params
            auto& lfoShape = *apvts.getRawParameterValue("LFO_SHAPE");
            auto lfoAmount = apvts.getRawParameterValue("LFO_AMOUNT")->load() / 100.0f;
            auto& lfoRate = *apvts.getRawParameterValue("LFO_RATE");
            auto& vibratoShape = *apvts.getRawParameterValue("VIB_SHAPE");
            auto vibratoAmount = apvts.getRawParameterValue("VIB_AMOUNT")->load() / 100.0f;
            auto& vibratoRate = *apvts.getRawParameterValue("VIB_RATE");
            voice->setLFOParams(juce::roundToInt(lfoShape.load()), lfoAmount, lfoRate.load(), 1);
            voice->setLFOParams(juce::roundToInt(vibratoShape.load()), vibratoAmount, vibratoRate.load(), 2);
            
            // ADSR Module Params
            auto& ampAtk = *apvts.getRawParameterValue("AMP_ATK");
            auto& ampDecay = *apvts.getRawParameterValue("AMP_DEC");
            auto ampSus = apvts.getRawParameterValue("AMP_SUS")->load() / 100.0f;
            auto& ampRel = *apvts.getRawParameterValue("AMP_REL");
            auto& filtAtk = *apvts.getRawParameterValue("FILT_ATK");
            auto& filtDecay = *apvts.getRawParameterValue("FILT_DEC");
            auto filtSus = apvts.getRawParameterValue("FILT_SUS")->load() / 100.0f;
            auto& filtRel = *apvts.getRawParameterValue("FILT_REL");
            voice->setAmpADSR(ampAtk.load(), ampDecay.load(), ampSus, ampRel.load());
            voice->setFilterADSR(filtAtk.load(), filtDecay.load(), filtSus, filtRel.load());
        }
    }
}

void FioreAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    // Clear garbage from buffers for when # outputs > # inputs. Otherwise feedback!
    for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i) {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
    
    // Update each voice with current values from our parameters
    // TODO: Figure out a way to save memory by only calling updates when needed!
    updateParams();
    
    // Get audio from the synth. Will call renderNextBlock for all the synth voices
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    const auto delayIsOn = apvts.getRawParameterValue("DELAY_ON")->load() > 0.5f;
    const auto delayTimeMs = apvts.getRawParameterValue("DELAY_TIME")->load();
    const auto delayFeedback = apvts.getRawParameterValue("DELAY_FEEDBACK")->load() / 100.0f;
    const auto delayWet = apvts.getRawParameterValue("DELAY_MIX")->load() / 100.0f;
    outputDelay.process(buffer, delayIsOn, delayTimeMs, delayFeedback, delayWet);

    const auto reverbIsOn = apvts.getRawParameterValue("REVERB_ON")->load() > 0.5f;
    const auto reverbRoom = apvts.getRawParameterValue("REVERB_ROOM")->load() / 100.0f;
    const auto reverbDamping = apvts.getRawParameterValue("REVERB_DAMPING")->load() / 100.0f;
    const auto reverbWet = apvts.getRawParameterValue("REVERB_MIX")->load() / 100.0f;
    outputReverb.process(buffer, reverbIsOn, reverbRoom, reverbDamping, reverbWet);

    const auto chorusIsOn = apvts.getRawParameterValue("CHORUS_ON")->load() > 0.5f;
    const auto chorusRate = apvts.getRawParameterValue("CHORUS_RATE")->load();
    const auto chorusDepth = apvts.getRawParameterValue("CHORUS_DEPTH")->load() / 100.0f;
    const auto chorusWet = apvts.getRawParameterValue("CHORUS_MIX")->load() / 100.0f;
    outputChorus.process(buffer, chorusIsOn, chorusRate, chorusDepth, chorusWet);

    const auto distortionIsOn = apvts.getRawParameterValue("DIST_ON")->load() > 0.5f;
    const auto distortionDrive = apvts.getRawParameterValue("DIST_DRIVE")->load() / 100.0f;
    const auto distortionTone = apvts.getRawParameterValue("DIST_TONE")->load() / 100.0f;
    const auto distortionWet = apvts.getRawParameterValue("DIST_MIX")->load() / 100.0f;
    outputDistortion.process(buffer, distortionIsOn, distortionDrive, distortionTone, distortionWet);
}

void FioreAudioProcessor::releaseResources() {
    
}

juce::AudioProcessorValueTreeState& FioreAudioProcessor::getAPVTS() {
    return apvts;
}

//==============================================================================
// JUCE Auto-Generated Stuff (don't touch)

const juce::String FioreAudioProcessor::getName() const {
    return JucePlugin_Name;
}

bool FioreAudioProcessor::acceptsMidi() const {
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FioreAudioProcessor::producesMidi() const {
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FioreAudioProcessor::isMidiEffect() const {
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FioreAudioProcessor::getTailLengthSeconds() const {
    return std::max(static_cast<double> (DelayEffect::maxDelayTimeSeconds), ReverbEffect::tailLengthSeconds);
}

int FioreAudioProcessor::getNumPrograms() {
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FioreAudioProcessor::getCurrentProgram() {
    return 0;
}

void FioreAudioProcessor::setCurrentProgram (int index) {
    juce::ignoreUnused(index);
}

const juce::String FioreAudioProcessor::getProgramName (int index) {
    juce::ignoreUnused(index);
    return {};
}

void FioreAudioProcessor::changeProgramName (int index, const juce::String& newName) {
    juce::ignoreUnused(index, newName);
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FioreAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const {
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

bool FioreAudioProcessor::hasEditor() const {
    return true;
}

juce::AudioProcessorEditor* FioreAudioProcessor::createEditor() {
    return new Editor (*this);
}

void FioreAudioProcessor::getStateInformation (juce::MemoryBlock& destData) {
    // From juce tutorial: https://docs.juce.com/master/tutorial_audio_processor_value_tree_state.html
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void FioreAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {
    // From juce tutorial: https://docs.juce.com/master/tutorial_audio_processor_value_tree_state.html
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr) {
        if (xmlState->hasTagName (apvts.state.getType())) {
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
        }
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new FioreAudioProcessor();
}
