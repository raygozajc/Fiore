# Fiore

![Fiore GUI](gui-sample.png "Fiore GUI")

### Overview 🎹
Fiore is a synthesizer plugin built with [JUCE](https://juce.com). Its layout and feature set were inspired by Logic Pro's Retro Synth. It's a simple subtractive synth containing the following features:

- Two **oscillators** with selectable waveforms and detuning capability for OSC2
- A **filter** (lowpass, bandpass, or highpass) with adjutable resonance and drive
- ADSR **envelopes** for amplitude and filter cutoff
- Two **LFOs** for filter cutoff modulation and vibrato, respectively
- 16-voice polyphony

I built this synth as a senior capstone project for my degree in Computer Science & Music at UIUC. 🎓

### Build

To build and run the synth from its source code with CMake:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
open "build/Fiore_artefacts/Debug/Standalone/Fiore.app"
```

The CMake build downloads JUCE 8.0.10 automatically on first configure and produces Standalone, VST3, and AU targets on macOS.
