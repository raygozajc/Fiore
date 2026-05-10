cmake_minimum_required(VERSION 3.22)

project(Fiore VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.10
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(JUCE)

set(PLUGIN_FORMATS Standalone VST3)

if(APPLE)
    list(APPEND PLUGIN_FORMATS AU)
endif()

juce_add_plugin(Fiore
    COMPANY_NAME "calesplugins"
    PRODUCT_NAME "Fiore"
    BUNDLE_ID "com.calesplugins.fiore"
    PLUGIN_MANUFACTURER_CODE "Cale"
    PLUGIN_CODE "Fior"
    FORMATS ${PLUGIN_FORMATS}
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
)

juce_generate_juce_header(Fiore)

if(TARGET Fiore_vst3_helper)
    target_compile_options(Fiore_vst3_helper PRIVATE -Wno-deprecated-declarations)
endif()

target_sources(Fiore
    PRIVATE
        Source/AmpModule.cpp
        Source/ChorusEffect.cpp
        Source/DelayEffect.cpp
        Source/DistortionEffect.cpp
        Source/EnvModule.cpp
        Source/EffectsRackModule.cpp
        Source/FilterModule.cpp
        Source/LFOVibratoModule.cpp
        Source/OscillatorModule.cpp
        Source/ReverbEffect.cpp
        Source/SynthVoice.cpp
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
)

target_compile_definitions(Fiore
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(Fiore
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
