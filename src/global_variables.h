// Global variables
//////////////////////////////////////////////////////////////////////

// set SYNTH_DEBUG to enable debug logging (1=most,2=all messages)
#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H
// LCD Buffers

char buffer_firstrow[16];
char buffer_secondrow[16];

//CC Controls///////////////////////////////

// OSC1
#define CC_Level1 0  // CC code for WF1
#define CC_OSC1 1    // CC code for WF1
#define CC_Octave1 2 // CC code for WF1
#define CC_PWM1 3    // CC code for WF1

// OSC2
#define CC_Level2 4  // CC code for WF1
#define CC_OSC2 5    // CC code for WF1
#define CC_Octave2 6 // CC code for WF1
#define CC_PWM2 7    // CC code for WF1

// Pitch LFO
#define CC_LFO_Level1 12 // LFO2 level
#define CC_LFO1 13       // LFO2 waveform
#define CC_LFO_Rate1 16  // LFO1 frequency

// Filter LFO
#define CC_LFO_Level2 17 // LFO2 level
#define CC_LFO2 18       // LFO2 waveform
#define CC_LFO_Rate2 21  // LFO2 frequency

// Master
#define CC_Volume 22     // Master Volume
#define CC_Pan 23        // Pan
#define CC_Portamento 24 // Glide
#define CC_Noise 25      // White Noise - No Noise - Pink Noise

//Env
#define CC_Attack 26  // Attack
#define CC_Release 27 // // Release
#define CC_Decay 28   // Decay
#define CC_Sustain 29 // Sustain

// Filter Envelope
#define CC_Filter_Attack 30
#define CC_Filter_Decay 31
#define CC_Filter_Sustain 32
#define CC_Filter_Release 33

//Filter
#define CC_Filter_Frequency 34   // CC code for volume
#define CC_Filter_Resonance 35   // CC code for volume
#define CC_Filter_Attenuation 36 // CC code for volume
#define CC_Filter_Mode 37

// Flanger
#define CC_Flanger_On 38
#define CC_Flanger_Offset 39
#define CC_Flanger_Depth 40
#define CC_Flanger_Fine 41

// Misc
#define CC_PWM_Rate 42 // Pulse Width Modulation rate
#define CC_Oscmix 43
#define CC_Detune 44 // OSC 2 detune
//#define CC_Detune2 45 // OSC 1 - BALANCED - OSC 2

//MIDI
#define CC_Sustain_Pedal 46 //
#define CC_Envelope_Mode 47 //

//Knobs
#define KNOB1 14 // LED1
#define KNOB2 16 // LED1
#define KNOB3 17 // LED1
#define KNOB4 18 // LED1

// PROGRAM CHANGE
#define MOD 15     // Input for modulators (osc, filter, adsr etc.)
#define PROGRAM 19 // Input for program (SAVE/RECALL)
#define THRESHOLD_SAVE 400
#define THRESHOLD_LOAD 1000
// LEDs
#define LED1 2  // LED1
#define LED2 3  // LED2
#define LED3 4  // LED3
#define LED4 5  // LED4
#define LED5 6  // LED5
#define LED6 20  // LED6
#define LED7 21 // LED7
#define LED8 22 // LED8
#define LED9 23 // LED9

#define PORTAMENTO 21 //
#define PAN 22        //

////////////////////////////////////////////////////////////

#define SYNTH_DEBUG 1
// define MIDI channel
#define SYNTH_MIDICHANNEL 1
// define tuning of A4 in Hz
#define SYNTH_TUNING 440
// gain at oscillator/filter input stage (1:1)
// keep low so filter does not saturate with resonance
#define GAIN_OSC 0.5
// gain in final mixer stage for polyphonic mode (4:1)
// (0.25 is the safe value but larger sounds better :) )
//#define GAIN_POLY 1.
#define GAIN_POLY 0.1
// gain in final mixer stage for monophonic modes
// define delay lines for modulation effects

#define DELAY_LENGTH (16 * AUDIO_BLOCK_SAMPLES)
short delaylineL[DELAY_LENGTH];
short delaylineR[DELAY_LENGTH];

//Filter frequencies
const float FILTERFREQS[128] = {20, 23, 26, 29, 32, 36, 40, 46, 53, 60, 69, 78, 87, 98, 109, 120, 132, 145, 157, 171, 186, 200, 215, 231, 247, 264, 282, 300, 319, 338, 357, 378, 399, 421, 444, 467, 491, 516, 541, 567, 594, 621, 650, 680, 710, 741, 774, 806, 841, 876, 912, 949, 987, 1027, 1068, 1110, 1152, 1196, 1242, 1290, 1338, 1388, 1439, 1491, 1547, 1603, 1661, 1723, 1783, 1843, 1915, 1975, 2047, 2119, 2191, 2263, 2347, 2419, 2503, 2587, 2683, 2767, 2863, 2959, 3055, 3163, 3259, 3367, 3487, 3595, 3715, 3835, 3967, 4099, 4231, 4363, 4507, 4663, 4807, 4963, 5131, 5287, 5467, 5635, 5815, 6007, 6199, 6403, 6607, 6823, 7039, 7267, 7495, 7735, 7987, 8239, 8503, 8779, 9055, 9343, 9643, 9955, 10267, 10603, 10939, 11287, 11647, 12000};

// audio memory
#define AMEMORY 100

#include <MIDI.h>

// switch between USB and UART MIDI
#ifdef USB_MIDI
#define SYNTH_SERIAL Serial1
#else // 'real' MIDI via UART
#define SYNTH_SERIAL Serial
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
#endif

float masterVolume = 1;

// Waveforms
uint8_t currentProgram1 = WAVEFORM_TRIANGLE; // OSC1 waveform
uint8_t currentProgram2 = WAVEFORM_SINE;     // OSC2 waveform
uint8_t currentProgram3 = WAVEFORM_TRIANGLE; // OSC3 waveform

int octave1; // OSC1 octave
int octave2; // OSC2 octave
int octave3; // OSC3 octave

float currentLevel1 = 0.75; //AMplitude OSC 1
float currentLevel2 = 0.75; // Amplitude OSC 2
float currentLevel3 = 0.75; // Amplitude OSC 3

//Bools
bool polyOn;
bool omniOn;
bool velocityOn;
bool sustainPressed;

float channelVolume;

// Panorama
float panorama;
float right; //right channel
float left;  // left channel

//Pulse Width
float pulseWidth1; // 0.05-0.95
float pulseWidth2; // 0.05-0.95

// LFO
uint8_t currentLFOProgram1 = WAVEFORM_SQUARE;
uint8_t currentLFOProgram2 = WAVEFORM_TRIANGLE;
float LFO1_Level;
float LFO2_Level;
float LFO3_Level;

float LFO1_Rate;
float LFO2_Rate;

float LFO_PWM_Freq;
float gain_pwm1;
float gain_pwm2;

float pitchBend; // -1/+1 oct
float pitchScale;
int octCorr;
int octCorr1;
int octCorr2;
int octCorr3;
float detune;  //Up to 1% detune
float detune2; //Up to 1% detune

// filter
int filtermode;
float filtFreq;   // 20-AUDIO_SAMPLE_RATE_EXACT/2.5
float filtReso;   // 0.9-5.0
float filtAtt;    // 0-1
float filtOctave; //Octave

//filter envelope
float FiltAttack;
float FiltDecay;
float FiltSustain;
float FiltRelease;

// envelope
const int envelopeMax = 500; //Ceiling for envelope
bool envOn;
float envDecay;   // 0-envelopeMax
float envAttack;  // 0-envelopeMax
float envSustain; // 0-1
float envRelease; // 0-envelopeMax

// Flanger
bool flangerOn;
int flangerOffset;
int flangerDepth;
float flangerFreqCoarse;
float flangerFreqFine;

// portamento
bool portamentoOn;
uint16_t portamentoTime;
int8_t portamentoDir;
float portamentoStep;
float portamentoPos;

// Master
int nvoices;
float noise;
#endif /* GLOBALVARIABLES_H */
