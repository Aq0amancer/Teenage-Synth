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
#include <iostream> // std::cout
#include <LCD.h>

// Debugging functions
//////////////////////////////////////////////////////////////////////

/// CUSTOM FUNCTION DECLARATIONS

void oscDump(uint8_t idx);
void oscDump(const Oscillator &o);
void printLCD(char *section, char *variable, int value, char *unit);
void checkKnobs();

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
  Serial.begin(115200);

  // LCD

  lcd.begin(16, 2);

  pinMode(KNOB1, INPUT);          // Knob 1
  pinMode(KNOB2, INPUT);          // Knob 2
  pinMode(KNOB3, INPUT);          // Knob 3
  pinMode(KNOB4, INPUT);          // Knob 4
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

  digitalWrite(LED1, 0);
  digitalWrite(LED2, 0);
  digitalWrite(LED3, 0);
  digitalWrite(LED4, 0);
  digitalWrite(LED5, 0);
  digitalWrite(LED6, 0);
  digitalWrite(LED7, 0);
  digitalWrite(LED8, 0);
  digitalWrite(LED9, 0);


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

  //usbMIDI.setHandleNoteOff(OnNoteOff);
  //usbMIDI.setHandleNoteOn(OnNoteOn);
  //usbMIDI.setHandleControlChange(OnControlChange);
  //usbMIDI.setHandlePitchChange(OnPitchChange);
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
  //usbMIDI.read();

#if SYNTH_DEBUG > 0
  performanceCheck();
  while (SYNTH_SERIAL.available())
    selectCommand(SYNTH_SERIAL.read());
#endif

  checkKnobs(); //Update controls
  delay(50);
}

void checkKnobs()
{
  static int controlThresh = 5;
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
    Serial.println(MODRead);
    if (MODRead > 135) //Any button clicked?
    {

      //sprintf(buffer_secondrow, "Mod Read %d", MODRead);
      

      if (MODRead < 170 && MODRead > 135) // Button 1 clicked
      {
        digitalWrite(LED1, 1);
        digitalWrite(LED2, 0);
        digitalWrite(LED3, 0);
        digitalWrite(LED4, 0);
        digitalWrite(LED5, 0);
        digitalWrite(LED6, 0);
        digitalWrite(LED7, 0);
        digitalWrite(LED8, 0);
        digitalWrite(LED9, 0);

        if (PROGRAMRead > THRESHOLD_SAVE && PROGRAMRead < THRESHOLD_LOAD) //SAVE clicked
        {
          //recallPatch(1);
        }
        else if (PROGRAMRead >= THRESHOLD_LOAD) //RECALL clicked
        {
          //savePatch(1);
        }
        else // CHANGE MOD
        {
          if (KNOB1Read > (KNOB1Value + controlThresh) || KNOB1Read < (KNOB1Value - controlThresh))
          {
            KNOB1Value=KNOB1Read;
            KNOB1Read = (KNOB1Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_Level1, KNOB1Read);
          }
          if (KNOB2Read > (KNOB2Value + controlThresh) || KNOB2Read < (KNOB2Value - controlThresh))
          {
            KNOB2Value=KNOB2Read;
            KNOB2Read = (KNOB2Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_OSC1, KNOB2Read);
          }
          if (KNOB3Read > (KNOB3Value + controlThresh) || KNOB3Read < (KNOB3Value - controlThresh))
          {
            KNOB3Value=KNOB3Read;
            KNOB3Read = (KNOB3Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_Octave1, KNOB3Read);
          }
          if (KNOB4Read > (KNOB4Value + controlThresh) || KNOB4Read < (KNOB4Value - controlThresh))
          {
            KNOB4Value=KNOB4Read;
            KNOB4Read = (KNOB4Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_PWM1, KNOB4Read);
          }
        }
      }
      else if (MODRead < 200 && MODRead >= 170) // Button 2 clicked
      {
        digitalWrite(LED1, 0);
        digitalWrite(LED2, 1);
        digitalWrite(LED3, 0);
        digitalWrite(LED4, 0);
        digitalWrite(LED5, 0);
        digitalWrite(LED6, 0);
        digitalWrite(LED7, 0);
        digitalWrite(LED8, 0);
        digitalWrite(LED9, 0);
        if (PROGRAMRead > THRESHOLD_SAVE && PROGRAMRead < THRESHOLD_LOAD) //SAVE clicked
        {
          //recallPatch(1);
        }
        else if (PROGRAMRead >= THRESHOLD_LOAD) //RECALL clicked
        {
          //savePatch(1);
        }
        else
        {
          if (KNOB1Read > (KNOB1Value + controlThresh) || KNOB1Read < (KNOB1Value - controlThresh))
          {
            KNOB1Value=KNOB1Read;
            KNOB1Read = (KNOB1Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_Level2, KNOB1Read);
          }
          if (KNOB2Read > (KNOB2Value + controlThresh) || KNOB2Read < (KNOB2Value - controlThresh))
          {
            KNOB2Value=KNOB2Read;
            KNOB2Read = (KNOB2Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_OSC2, KNOB2Read);
          }
          if (KNOB3Read > (KNOB3Value + controlThresh) || KNOB3Read < (KNOB3Value - controlThresh))
          {
            KNOB3Value=KNOB3Read;
            KNOB3Read = (KNOB3Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_Octave2, KNOB3Read);
          }
          if (KNOB4Read > (KNOB4Value + controlThresh) || KNOB4Read < (KNOB4Value - controlThresh))
          {
            KNOB4Value=KNOB4Read;
            KNOB4Read = (KNOB4Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_PWM2, KNOB4Read);
          }
        }
      }
      else if (MODRead < 225 && MODRead >= 200) // Button 3 clicked
      {
        digitalWrite(LED1, 0);
        digitalWrite(LED2, 0);
        digitalWrite(LED3, 1);
        digitalWrite(LED4, 0);
        digitalWrite(LED5, 0);
        digitalWrite(LED6, 0);
        digitalWrite(LED7, 0);
        digitalWrite(LED8, 0);
        digitalWrite(LED9, 0);
        if (PROGRAMRead > THRESHOLD_SAVE && PROGRAMRead < THRESHOLD_LOAD) //SAVE clicked
        {
          //recallPatch(1);
        }
        else if (PROGRAMRead >= THRESHOLD_LOAD) //RECALL clicked
        {
          //savePatch(1);
        }
        else
        { // CHANGE MOD
        }
      }

      else if (MODRead < 255 && MODRead >= 225) // Button 4 clicked
      {
        digitalWrite(LED1, 0);
        digitalWrite(LED2, 0);
        digitalWrite(LED3, 0);
        digitalWrite(LED4, 1);
        digitalWrite(LED5, 0);
        digitalWrite(LED6, 0);
        digitalWrite(LED7, 0);
        digitalWrite(LED8, 0);
        digitalWrite(LED9, 0);

        if (PROGRAMRead > THRESHOLD_SAVE && PROGRAMRead < THRESHOLD_LOAD) //SAVE clicked
        {
          //recallPatch(1);
        }
        else if (PROGRAMRead >= THRESHOLD_LOAD) //RECALL clicked
        {
          //savePatch(1);
        }
        else
        { // CHANGE MOD
        }
      }
      else if (MODRead < 300 && MODRead >= 255) // Button 5 clicked
      {
        digitalWrite(LED1, 0);
        digitalWrite(LED2, 0);
        digitalWrite(LED3, 0);
        digitalWrite(LED4, 0);
        digitalWrite(LED5, 1);
        digitalWrite(LED6, 0);
        digitalWrite(LED7, 0);
        digitalWrite(LED8, 0);
        digitalWrite(LED9, 0);
        if (PROGRAMRead > THRESHOLD_SAVE && PROGRAMRead < THRESHOLD_LOAD) //SAVE clicked
        {
          //recallPatch(1);
        }
        else if (PROGRAMRead >= THRESHOLD_LOAD) //RECALL clicked
        {
          //savePatch(1);
        }
        else
        { // CHANGE MOD
        }
      }
      else if (MODRead < 370 && MODRead >= 300) // Button 6 clicked
      {
        digitalWrite(LED1, 0);
        digitalWrite(LED2, 0);
        digitalWrite(LED3, 0);
        digitalWrite(LED4, 0);
        digitalWrite(LED5, 0);
        digitalWrite(LED6, 1);
        digitalWrite(LED7, 0);
        digitalWrite(LED8, 0);
        digitalWrite(LED9, 0);

        if (PROGRAMRead > THRESHOLD_SAVE && PROGRAMRead < THRESHOLD_LOAD) //SAVE clicked
        {
          //recallPatch(1);
        }
        else if (PROGRAMRead >= THRESHOLD_LOAD) //RECALL clicked
        {
          //savePatch(1);
        }
        else
        { // CHANGE MOD
        }
      }
      else if (MODRead < 500 && MODRead >= 370) // Button 7 clicked
      {
        digitalWrite(LED1, 0);
        digitalWrite(LED2, 0);
        digitalWrite(LED3, 0);
        digitalWrite(LED4, 0);
        digitalWrite(LED5, 0);
        digitalWrite(LED6, 0);
        digitalWrite(LED7, 1);
        digitalWrite(LED8, 0);
        digitalWrite(LED9, 0);
        if (PROGRAMRead > THRESHOLD_SAVE && PROGRAMRead < THRESHOLD_LOAD) //SAVE clicked
        {
          //recallPatch(1);
        }
        else if (PROGRAMRead >= THRESHOLD_LOAD) //RECALL clicked
        {
          //savePatch(1);
        }
        else
        { // CHANGE MOD
        }
      }
      else if (MODRead < 800 && MODRead >= 500) // Button 8 clicked
      {
        digitalWrite(LED1, 0);
        digitalWrite(LED2, 0);
        digitalWrite(LED3, 0);
        digitalWrite(LED4, 0);
        digitalWrite(LED5, 0);
        digitalWrite(LED6, 0);
        digitalWrite(LED7, 0);
        digitalWrite(LED8, 1);
        digitalWrite(LED9, 0);
        if (PROGRAMRead > THRESHOLD_SAVE && PROGRAMRead < THRESHOLD_LOAD) //SAVE clicked
        {
          //recallPatch(1);
        }
        else if (PROGRAMRead >= THRESHOLD_LOAD) //RECALL clicked
        {
          //savePatch(1);
        }
        else
        { // CHANGE MOD
        }
      }
      else if (MODRead >= 1000) // Button 9 clicked
      {
        digitalWrite(LED1, 0);
        digitalWrite(LED2, 0);
        digitalWrite(LED3, 0);
        digitalWrite(LED4, 0);
        digitalWrite(LED5, 0);
        digitalWrite(LED6, 0);
        digitalWrite(LED7, 0);
        digitalWrite(LED8, 0);
        digitalWrite(LED9, 1);
        if (PROGRAMRead > THRESHOLD_SAVE && PROGRAMRead < THRESHOLD_LOAD) //SAVE clicked
        {
          //recallPatch(1);
        }
        else if (PROGRAMRead >= THRESHOLD_LOAD) //RECALL clicked
        {
          //savePatch(1);
        }
        else
        { // CHANGE MOD
        }
      }
    }
    else
    {
      digitalWrite(LED1, 0);
      digitalWrite(LED2, 0);
      digitalWrite(LED3, 0);
      digitalWrite(LED4, 0);
      digitalWrite(LED5, 0);
      digitalWrite(LED6, 0);
      digitalWrite(LED7, 0);
      digitalWrite(LED8, 0);
      digitalWrite(LED9, 0);
      lcd.clear();
    }

  }
}