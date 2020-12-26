#include <Arduino.h>
#include <Audio.h>
#include "saw.h"
#include "data_types_lookup_tables.h"
#include "global_variables.h"
#include "sounding_pressed_notes.h"
#include "parameter_control_functions.h"
#include "midi_handlers.h"
#include "SynthArch.h"
#include "global_variables.h"
#include "Wire.h"
#include <SPI.h>
#include <MIDI.h>
#include <SerialFlash.h>
#include <string>

// Debugging functions
//////////////////////////////////////////////////////////////////////

/// CUSTOM FUNCTION
void oscDump(uint8_t idx);
void oscDump(const Oscillator &o);
void printLCD_int(char *section, char *variable, int value, char *unit);
void printLCD_float(char *section, char *variable, float value, char *unit);
void printLCD_string(char *section, char *variable, char *value, char *unit);
////
#if SYNTH_DEBUG > 0
float statsCpu = 0;
uint8_t statsMem = 0;

void oscDump(uint8_t idx)
{
  SYNTH_SERIAL.print("Oscillator ");
  SYNTH_SERIAL.print(idx);
  oscDump(oscs[idx]);
}

void oscDump(const Oscillator &o)
{
  SYNTH_SERIAL.print(" note=");
  SYNTH_SERIAL.print(o.note);
  SYNTH_SERIAL.print(", velocity=");
  SYNTH_SERIAL.println(o.velocity);
}

inline void notesDump(int8_t *notes)
{
  for (uint8_t i = 0; i < NVOICES; ++i)
  {
    SYNTH_SERIAL.print(' ');
    SYNTH_SERIAL.print(notes[i]);
  }
  SYNTH_SERIAL.println();
}

inline void printResources(float cpu, uint8_t mem)
{
  SYNTH_SERIAL.print("CPU Usage: ");
  SYNTH_SERIAL.print(cpu);
  SYNTH_SERIAL.print("%, Memory: ");
  SYNTH_SERIAL.println(mem);
}

void performanceCheck()
{
  static unsigned long last = 0;
  unsigned long now = millis();
  if ((now - last) > 1000)
  {
    last = now;
    float cpu = AudioProcessorUsageMax();
    uint8_t mem = AudioMemoryUsageMax();
    if ((statsMem != mem) || fabs(statsCpu - cpu) > 1)
    {
      printResources(cpu, mem);
    }
    AudioProcessorUsageMaxReset();
    AudioMemoryUsageMaxReset();
    last = now;
    statsCpu = cpu;
    statsMem = mem;
  }
}

void printInfo()
{
  SYNTH_SERIAL.println();
  SYNTH_SERIAL.print("Master Volume:        ");
  SYNTH_SERIAL.println(masterVolume);
  SYNTH_SERIAL.print("Waveform 1:      ");
  SYNTH_SERIAL.println((String)progs[currentProgram1]);
  SYNTH_SERIAL.print("Waveform 2:      ");
  SYNTH_SERIAL.println((String)progs[currentProgram2]);
  SYNTH_SERIAL.print("Poly On:              ");
  SYNTH_SERIAL.println(polyOn);
  SYNTH_SERIAL.print("Omni On:              ");
  SYNTH_SERIAL.println(omniOn);
  SYNTH_SERIAL.print("Velocity On:          ");
  SYNTH_SERIAL.println(velocityOn);
  SYNTH_SERIAL.println();
  SYNTH_SERIAL.print("Sustain Pressed:      ");
  SYNTH_SERIAL.println(sustainPressed);
  SYNTH_SERIAL.print("Channel Volume:       ");
  SYNTH_SERIAL.println(channelVolume);
  SYNTH_SERIAL.print("Panorama:             ");
  SYNTH_SERIAL.println(panorama);
  SYNTH_SERIAL.print("Pitch Bend:           ");
  SYNTH_SERIAL.println(pitchBend);
  SYNTH_SERIAL.println();
  SYNTH_SERIAL.print("Filter Mode:          ");
  SYNTH_SERIAL.println(filtermode);
  SYNTH_SERIAL.print("Filter Frequency:     ");
  SYNTH_SERIAL.println(filtFreq);
  SYNTH_SERIAL.print("Filter Resonance:     ");
  SYNTH_SERIAL.println(filtReso);
  SYNTH_SERIAL.print("Filter Attenuation:   ");
  SYNTH_SERIAL.println(filtAtt);
  SYNTH_SERIAL.println();
  SYNTH_SERIAL.print("Envelope On:          ");
  SYNTH_SERIAL.println(envOn);
  SYNTH_SERIAL.print("Envelope Attack:      ");
  SYNTH_SERIAL.println(envAttack);
  /*
  SYNTH_SERIAL.print("Envelope Delay:       ");
  SYNTH_SERIAL.println(envDelay);
  SYNTH_SERIAL.print("Envelope Hold:        ");
  SYNTH_SERIAL.println(envHold);
  SYNTH_SERIAL.print("Envelope Decay:       ");
    */
  SYNTH_SERIAL.println(envDecay);
  SYNTH_SERIAL.print("Envelope Sustain:     ");
  SYNTH_SERIAL.println(envSustain);
  SYNTH_SERIAL.print("Envelope Release:     ");
  SYNTH_SERIAL.println(envRelease);
  SYNTH_SERIAL.println();
  SYNTH_SERIAL.print("Flanger On:           ");
  SYNTH_SERIAL.println(flangerOn);
  SYNTH_SERIAL.print("Flanger Offset:       ");
  SYNTH_SERIAL.println(flangerOffset);
  SYNTH_SERIAL.print("Flanger Depth:        ");
  SYNTH_SERIAL.println(flangerDepth);
  SYNTH_SERIAL.print("Flanger Freq. Coarse: ");
  SYNTH_SERIAL.println(flangerFreqCoarse);
  SYNTH_SERIAL.print("Flanger Freq. Fine:   ");
  SYNTH_SERIAL.println(flangerFreqFine);
  SYNTH_SERIAL.print("Delay Line Length:    ");
  SYNTH_SERIAL.println(DELAY_LENGTH);
  SYNTH_SERIAL.println();
  SYNTH_SERIAL.print("Portamento On:        ");
  SYNTH_SERIAL.println(portamentoOn);
  SYNTH_SERIAL.print("Portamento Time:      ");
  SYNTH_SERIAL.println(portamentoTime);
  SYNTH_SERIAL.print("Portamento Step:      ");
  SYNTH_SERIAL.println(portamentoStep);
  SYNTH_SERIAL.print("Portamento Direction: ");
  SYNTH_SERIAL.println(portamentoDir);
  SYNTH_SERIAL.print("Portamento Position:  ");
  SYNTH_SERIAL.println(portamentoPos);
}

void selectCommand(char c)
{
  switch (c)
  {
  case '\r':
    SYNTH_SERIAL.println();
    break;
  case 'b':
    // print voice statistics
    SYNTH_SERIAL.print("Notes Pressed:");
    notesDump(notesPressed);
    SYNTH_SERIAL.print("Notes On:     ");
    notesDump(notesOn);
    break;
  case 'o':
    // print oscillator status
    for (uint8_t i = 0; i < NVOICES; ++i)
      oscDump(i);
    break;
  case 's':
    // print cpu and mem usage
    printResources(statsCpu, statsMem);
    break;
  case 'r':
    // reset parameters
    resetAll();
    break;
  case 'i':
    // print info
    printInfo();
    break;
  case '\t':
    // reboot Teensy
    *(uint32_t *)0xE000ED0C = 0x5FA0004;
    break;
  case ' ':
    // send note off
    allOff();
    break;
  default:
    break;
  }
}

#endif

///////////////////////////////// SETUP ///////////////////////////////

void setup()
{
  // LCD

  lcd.begin(16, 2);

  pinMode(KNOB1, INPUT);   // Knob 1
  pinMode(KNOB2, INPUT);   // Knob 2
  pinMode(KNOB3, INPUT);   // Knob 3
  pinMode(KNOB4, INPUT);   // Knob 4
  pinMode(MOD, INPUT);     // Input for modulators (osc, filter, adsr etc.)
  pinMode(PROGRAM, INPUT); // Input for program (SAVE/RECALL)

  pinMode(LED1, OUTPUT); // LED1
  pinMode(LED2, OUTPUT); // LED2
  pinMode(LED3, OUTPUT); // LED3
  pinMode(LED4, OUTPUT); // LED4
  pinMode(LED5, OUTPUT); // LED5
  pinMode(LED6, OUTPUT); // LED6
  pinMode(LED7, OUTPUT); // LED7
  pinMode(LED8, OUTPUT); // LED8
  pinMode(LED9, OUTPUT); // LED9

#if SYNTH_DEBUG > 0
  SYNTH_SERIAL.begin(115200);
#endif

  AudioMemory(AMEMORY);
  sgtl5000_1.enable();
  sgtl5000_1.volume(masterVolume);

  {
    Oscillator *o = oscs, *end = oscs + NVOICES;
    do
    {
      o->wf1->arbitraryWaveform(reinterpret_cast<const int16_t *>(saw), 0);
      o->wf2->arbitraryWaveform(reinterpret_cast<const int16_t *>(saw), 0);
    } while (++o < end);
  }

  resetAll();

  MIDI.begin();
  MIDI.setHandleNoteOff(OnNoteOff);
  MIDI.setHandleNoteOn(OnNoteOn);
  MIDI.setHandleAfterTouchPoly(OnAfterTouchPoly);
  MIDI.setHandleControlChange(OnControlChange);
  MIDI.setHandlePitchBend(OnPitchChange);
  MIDI.setHandleAfterTouchChannel(OnAfterTouch);

  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleControlChange(OnControlChange);
  usbMIDI.setHandlePitchChange(OnPitchChange);
  delay(1000);

  SYNTH_SERIAL.println();
  SYNTH_SERIAL.println("TeensySynth v0.1");
#ifdef USB_MIDI
  SYNTH_SERIAL.println("USB_MIDI enabled");
#else
  SYNTH_SERIAL.println("UART_MIDI enabled");
#endif // USB_MIDI
}

/////////////////////// LOOP ////////////////////////////////////////////////////

void loop()
{
  MIDI.read();
  usbMIDI.read();

#if SYNTH_DEBUG > 0
  performanceCheck();
  while (SYNTH_SERIAL.available())
    selectCommand(SYNTH_SERIAL.read());
#endif

  checkKnobs(); //Update controls
  //delay(10);
}

void checkKnobs()
{
  static int controlThresh = 2;
  unsigned long currentMicros = micros();
  static unsigned long LFOtime = 0;
  static int KNOB1Value = {};
  static int KNOB2Value = {};
  static int KNOB3Value = {};
  static int KNOB4Value = {};

  if (currentMicros - LFOtime >= 1000)
  {
    LFOtime = currentMicros;

    int MODRead = analogRead(MOD);
    int KNOB1Read = analogRead(KNOB1);
    int KNOB2Read = analogRead(KNOB2);
    int KNOB3Read = analogRead(KNOB3);
    int KNOB4Read = analogRead(KNOB4);
    int PROGRAMRead = analogRead(PROGRAM);

    if (MODRead > 0)
    { //If clicking one of the 9 buttons
      if (MODRead < 114)
      { // Button 1 clicked
        if (PROGRAMRead > 0 && PROGRAMRead < 512)
        { //SAVE
          //recallPatch(1);
        }
        else if (PROGRAMRead >= 512)
        { //RECALL
          //savePatch(1);
        }
        else
        { // CHANGE MOD
          if (KNOB1Read > (KNOB1Value + controlThresh) || KNOB1Read < (KNOB1Value - controlThresh))
          {
            KNOB1Read = (KNOB1Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_Filter_Release, KNOB1Read);
          }
          if (KNOB2Read > (KNOB2Value + controlThresh) || KNOB2Read < (KNOB2Value - controlThresh))
          {
            KNOB2Read = (KNOB2Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_Filter_Release, KNOB2Read);
          }
          if (KNOB3Read > (KNOB3Value + controlThresh) || KNOB3Read < (KNOB3Value - controlThresh))
          {
            KNOB3Read = (KNOB3Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_Filter_Release, KNOB3Read);
          }
          if (KNOB4Read > (KNOB4Value + controlThresh) || KNOB4Read < (KNOB4Value - controlThresh))
          {
            KNOB4Read = (KNOB4Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_Filter_Release, KNOB4Read);
          }
        }
      }
      else if (MODRead < 228 && MODRead >= 114)
      { // Button 2 clicked
        if (PROGRAMRead > 0 && PROGRAMRead < 512)
        { //SAVE
        }
        else if (PROGRAMRead >= 512)
        { //RECALL
        }
        else
        { // CHANGE MOD
        }
      }
      else if (MODRead < 342 && MODRead >= 228)
      { // Button 3 clicked
        if (PROGRAMRead > 0 && PROGRAMRead < 512)
        { //SAVE
        }
        else if (PROGRAMRead >= 512)
        { //RECALL
        }
        else
        { // CHANGE MOD
        }
      }
      else if (MODRead < 456 && MODRead >= 342)
      { // Button 4 clicked
        if (PROGRAMRead > 0 && PROGRAMRead < 512)
        { //SAVE
        }
        else if (PROGRAMRead >= 512)
        { //RECALL
        }
        else
        { // CHANGE MOD
        }
      }
      else if (MODRead < 570 && MODRead >= 456)
      { // Button 5 clicked
        if (PROGRAMRead > 0 && PROGRAMRead < 512)
        { //SAVE
        }
        else if (PROGRAMRead >= 512)
        { //RECALL
        }
        else
        { // CHANGE MOD
        }
      }
      else if (MODRead < 684 && MODRead >= 570)
      { // Button 6 clicked
        if (PROGRAMRead > 0 && PROGRAMRead < 512)
        { //SAVE
        }
        else if (PROGRAMRead >= 512)
        { //RECALL
        }
        else
        { // CHANGE MOD
        }
      }
      else if (MODRead < 798 && MODRead >= 684)
      { // Button 7 clicked
        if (PROGRAMRead > 0 && PROGRAMRead < 512)
        { //SAVE
        }
        else if (PROGRAMRead >= 512)
        { //RECALL
        }
        else
        { // CHANGE MOD
        }
      }
      else if (MODRead < 912 && MODRead >= 798)
      { // Button 8 clicked
        if (PROGRAMRead > 0 && PROGRAMRead < 512)
        { //SAVE
        }
        else if (PROGRAMRead >= 512)
        { //RECALL
        }
        else
        { // CHANGE MOD
        }
      }
      else if (MODRead >= 912)
      { // Button 9 clicked
        if (PROGRAMRead > 0 && PROGRAMRead < 512)
        { //SAVE
        }
        else if (PROGRAMRead >= 512)
        { //RECALL
        }
        else
        { // CHANGE MOD
        }
      }
    }
  }

case 0:
  OnControlChange(SYNTH_MIDICHANNEL, CC_Filter_Release, muxRead);
  break;
case 1:
  OnControlChange(SYNTH_MIDICHANNEL, CC_Filter_Attenuation, muxRead);
  break;
case 2:
  OnControlChange(SYNTH_MIDICHANNEL, CC_Filter_Mode, muxRead);
  break;
case 3:
  OnControlChange(SYNTH_MIDICHANNEL, CC_Filter_Resonance, muxRead);
  break;
case 4:
  OnControlChange(SYNTH_MIDICHANNEL, CC_Volume, muxRead);
  break;
case 5:
  OnControlChange(SYNTH_MIDICHANNEL, CC_Waveform1, muxRead);
  break;
case 6:
  OnControlChange(SYNTH_MIDICHANNEL, CC_Waveform2, muxRead);
  break;
case 7:
  OnControlChange(SYNTH_MIDICHANNEL, CC_Pan, muxRead);
  break;
}
}

if (muxRead2 > (muxValues2[muxInput] + controlThresh) || muxRead2 < (muxValues2[muxInput] - controlThresh))
{
  muxValues2[muxInput] = muxRead2;
  muxRead2 = (muxRead2 >> 3); //Change range to 0-127
  switch (muxInput)
  {
  case 0:
    OnControlChange(SYNTH_MIDICHANNEL, CC_Filter_Frequency, muxRead2);
    break;
  case 1:
    OnControlChange(SYNTH_MIDICHANNEL, CC_Octave1, muxRead2);
    break;
  case 2:
    OnControlChange(SYNTH_MIDICHANNEL, CC_Detune, muxRead2);
    break;
  case 3:
    OnControlChange(SYNTH_MIDICHANNEL, CC_Attack, muxRead2);
    break;
  case 4:
    OnControlChange(SYNTH_MIDICHANNEL, CC_Decay, muxRead2);
    break;
  case 5:
    OnControlChange(SYNTH_MIDICHANNEL, CC_Sustain, muxRead2);
    break;
  case 6:
    OnControlChange(SYNTH_MIDICHANNEL, CC_Release, muxRead2);
    break;
  case 7:
    OnControlChange(SYNTH_MIDICHANNEL, CC_Flanger_On, muxRead2);
    break;
  }
}
if (muxRead3 > (muxValues3[muxInput] + controlThresh) || muxRead3 < (muxValues3[muxInput] - controlThresh))
{
  muxValues3[muxInput] = muxRead3;
  muxRead3 = (muxRead3 >> 3); //Change range to 0-127
  switch (muxInput)
  {
  case 0:
    OnControlChange(SYNTH_MIDICHANNEL, CC_Flanger_Depth, muxRead3);
    break;
  case 1:
    OnControlChange(SYNTH_MIDICHANNEL, CC_Flanger_Offset, muxRead3);
    break;
  case 2:
    OnControlChange(SYNTH_MIDICHANNEL, CC_Flanger_Fine, muxRead3);
    break;
  case 3:
    OnControlChange(SYNTH_MIDICHANNEL, CC_Noise, muxRead3);
    break;
  case 4:
    OnControlChange(SYNTH_MIDICHANNEL, CC_PWM1, muxRead3);
    break;
  case 5:
    OnControlChange(SYNTH_MIDICHANNEL, CC_PWM2, muxRead3);
    break;
  case 6:
    OnControlChange(SYNTH_MIDICHANNEL, CC_Oscmix, muxRead3);
    break;
  case 7:
    OnControlChange(SYNTH_MIDICHANNEL, CC_PWM_Rate, muxRead3);
    break;
  }
}
if (muxRead4 > (muxValues4[muxInput] + controlThresh) || muxRead4 < (muxValues4[muxInput] - controlThresh))
{
  muxValues4[muxInput] = muxRead4;
  muxRead4 = (muxRead4 >> 3); //Change range to 0-127
  switch (muxInput)
  {
  case 0:
    OnControlChange(SYNTH_MIDICHANNEL, CC_Filter_Attack, muxRead4);
    break;
  case 1:
    OnControlChange(SYNTH_MIDICHANNEL, CC_Octave2, muxRead4);
    break;
  case 2:
    OnControlChange(SYNTH_MIDICHANNEL, CC_LFO_Level1, muxRead4);
    break;
  case 3:
    OnControlChange(SYNTH_MIDICHANNEL, CC_LFO_Waveform1, muxRead4);
    break;
  case 4:
    OnControlChange(SYNTH_MIDICHANNEL, CC_LFO_Rate1, muxRead4);
    break;
  case 5:
    OnControlChange(SYNTH_MIDICHANNEL, CC_LFO_Level2, muxRead4);
    break;
  case 6:
    OnControlChange(SYNTH_MIDICHANNEL, CC_LFO_Waveform2, muxRead4);
    break;
  case 7:
    OnControlChange(SYNTH_MIDICHANNEL, CC_LFO_Rate2, muxRead4);
    break;
  }
}
}