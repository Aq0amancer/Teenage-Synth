//////////////////////////////////////////////////////////////////////
// Data types and lookup tables
//////////////////////////////////////////////////////////////////////
#ifndef DATATYPES_H
#define DATATYPES_H

#include "SynthArch.h"

struct Oscillator
{
    AudioSynthWaveformModulated *wf1;
    AudioSynthWaveformModulated *wf2;
    AudioMixer4 *wfmix;
    AudioFilterStateVariable *filt;
    AudioMixer4 *mix;
    AudioEffectEnvelope *env;
    int8_t note;
    uint8_t velocity;
};

#define NVOICES 8 // 8 voice polyphony, needs 8 waveforms, filters, mixers and envelopes
Oscillator oscs[NVOICES] = {
    {&waveform1, &waveform2, &mixer9, &filter1, &mixer1, &envelope1, -1, 0},
    {&waveform3, &waveform4, &mixer10, &filter2, &mixer2, &envelope2, -1, 0},
    {&waveform5, &waveform6, &mixer11, &filter3, &mixer3, &envelope3, -1, 0},
    {&waveform7, &waveform8, &mixer12, &filter4, &mixer4, &envelope4, -1, 0},
    {&waveform9, &waveform10, &mixer13, &filter5, &mixer5, &envelope5, -1, 0},
    {&waveform11, &waveform12, &mixer14, &filter6, &mixer6, &envelope6, -1, 0},
    {&waveform13, &waveform14, &mixer15, &filter7, &mixer7, &envelope7, -1, 0},
    {&waveform15, &waveform16, &mixer16, &filter8, &mixer8, &envelope8, -1, 0},
};
#define NPROGS 7
uint8_t progs[NPROGS] = {
    WAVEFORM_SAWTOOTH,
    WAVEFORM_SQUARE,
    WAVEFORM_TRIANGLE,
    WAVEFORM_SINE,
    WAVEFORM_ARBITRARY,
    WAVEFORM_SAWTOOTH_REVERSE,
    WAVEFORM_PULSE};

// Waveforms
const char *waveforms[NPROGS] = {
    "Saw",
    "Square",
    "Tri",
    "Sine",
    "Rand",
    "SawRev",
    "Pulse"};
#endif /* DATATYPES_H */
