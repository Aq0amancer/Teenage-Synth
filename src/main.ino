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

//void oscDump(uint8_t idx);
//void oscDump(const Oscillator &o);
//void printLCD(char *section, char *variable, int value, char *unit);
//void checkKnobs();

////
#if SYNTH_DEBUG > 0
float statsCpu = 0;
uint8_t statsMem = 0;

void oscDump(uint8_t idx)
{
  Serial.print("Oscillator ");
  Serial.print(idx);
  oscDump(oscs[idx]);
}

void oscDump(const Oscillator &o)
{
  Serial.print(" note=");
  Serial.print(o.note);
  Serial.print(", velocity=");
  Serial.println(o.velocity);
}

inline void notesDump(int8_t *notes)
{
  for (uint8_t i = 0; i < NVOICES; ++i)
  {
    Serial.print(' ');
    Serial.print(notes[i]);
  }
  Serial.println();
}

inline void printResources(float cpu, uint8_t mem)
{
  Serial.print("CPU Usage: ");
  Serial.print(cpu);
  Serial.print("%, Memory: ");
  Serial.println(mem);
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
  Serial.println();
  Serial.print("Master Volume:        ");
  Serial.println(masterVolume);
  Serial.print("Waveform 1:      ");
  Serial.println((String)progs[currentProgram1]);
  Serial.print("Waveform 2:      ");
  Serial.println((String)progs[currentProgram2]);
  Serial.print("Poly On:              ");
  Serial.println(polyOn);
  Serial.print("Omni On:              ");
  Serial.println(omniOn);
  Serial.print("Velocity On:          ");
  Serial.println(velocityOn);
  Serial.println();
  Serial.print("Sustain Pressed:      ");
  Serial.println(sustainPressed);
  Serial.print("Channel Volume:       ");
  Serial.println(channelVolume);
  Serial.print("Panorama:             ");
  Serial.println(panorama);
  Serial.print("Pitch Bend:           ");
  Serial.println(pitchBend);
  Serial.println();
  Serial.print("Filter Mode:          ");
  Serial.println(filtermode);
  Serial.print("Filter Frequency:     ");
  Serial.println(filtFreq);
  Serial.print("Filter Resonance:     ");
  Serial.println(filtReso);
  Serial.print("Filter Attenuation:   ");
  Serial.println(filtAtt);
  Serial.println();
  Serial.print("Envelope On:          ");
  Serial.println(envOn);
  Serial.print("Envelope Attack:      ");
  Serial.println(envAttack);
  /*
  Serial.print("Envelope Delay:       ");
  Serial.println(envDelay);
  Serial.print("Envelope Hold:        ");
  Serial.println(envHold);
  Serial.print("Envelope Decay:       ");
    */
  Serial.println(envDecay);
  Serial.print("Envelope Sustain:     ");
  Serial.println(envSustain);
  Serial.print("Envelope Release:     ");
  Serial.println(envRelease);
  Serial.println();
  Serial.print("Flanger On:           ");
  Serial.println(flangerOn);
  Serial.print("Flanger Offset:       ");
  Serial.println(flangerOffset);
  Serial.print("Flanger Depth:        ");
  Serial.println(flangerDepth);
  Serial.print("Flanger Freq. Coarse: ");
  Serial.println(flangerFreqCoarse);
  Serial.print("Flanger Freq. Fine:   ");
  Serial.println(flangerFreqFine);
  Serial.print("Delay Line Length:    ");
  Serial.println(DELAY_LENGTH);
  Serial.println();
  Serial.print("Portamento On:        ");
  Serial.println(portamentoOn);
  Serial.print("Portamento Time:      ");
  Serial.println(portamentoTime);
  Serial.print("Portamento Step:      ");
  Serial.println(portamentoStep);
  Serial.print("Portamento Direction: ");
  Serial.println(portamentoDir);
  Serial.print("Portamento Position:  ");
  Serial.println(portamentoPos);
}

void selectCommand(char c)
{
  switch (c)
  {
  case '\r':
    Serial.println();
    break;
  case 'b':
    // print voice statistics
    Serial.print("Notes Pressed:");
    notesDump(notesPressed);
    Serial.print("Notes On:     ");
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
  lcd.begin(16, 2);

  pinMode(KNOB1, INPUT);          // Knob 1
  pinMode(KNOB2, INPUT);          // Knob 2
  pinMode(KNOB3, INPUT);          // Knob 3
  pinMode(KNOB4, INPUT);          // Knob 4
  pinMode(MOD, INPUT);     // Input for modulators (osc, filter, adsr etc.)
  pinMode(PROGRAM, INPUT); // Input for program (SAVE/RECALL)


#if SYNTH_DEBUG > 0
  Serial.begin(115200);
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

  //MIDI.begin();
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleAfterTouchPoly(OnAfterTouchPoly);
  usbMIDI.setHandleControlChange(OnControlChange);
  usbMIDI.setHandlePitchChange(OnPitchChange);
  usbMIDI.setHandleAfterTouchChannel(OnAfterTouch);

  //usbMIDI.setHandleNoteOff(OnNoteOff);
  //usbMIDI.setHandleNoteOn(OnNoteOn);
  //usbMIDI.setHandleControlChange(OnControlChange);
  //usbMIDI.setHandlePitchChange(OnPitchChange);
  delay(1000);

  Serial.println();
  Serial.println("TeensySynth v0.1");
#ifdef USB_MIDI
  Serial.println("USB_MIDI enabled");
#else
  Serial.println("UART_MIDI enabled");
#endif // USB_MIDI
}

/////////////////////// LOOP ////////////////////////////////////////////////////

void loop()
{

#if SYNTH_DEBUG > 0
  performanceCheck();
  if (usbMIDI.read())
    selectCommand(usbMIDI.read());
#endif

  checkKnobs(); //Update controls
  delay(50);
}

void checkKnobs()
{
  static int controlThresh = 30;
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

    //Serial.println(KNOB4Read);

    if (MODRead > 50) //Any button clicked?
    {

      //sprintf(buffer_secondrow, "Mod Read %d", MODRead);
      

      if (MODRead < 70 && MODRead > 50) // Button 1 clicked
      {
        //1, 1);
        //2, 0);
        //3, 0);
        //4, 0);
        //5, 0);
        //6, 0);
        //7, 0);
        //8, 0);
        //9, 0);

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
      else if (MODRead < 100 && MODRead >= 80) // Button 2 clicked
      {
        //1, 0);
        //2, 1);
        //3, 0);
        //4, 0);
        //5, 0);
        //6, 0);
        //7, 0);
        //8, 0);
        //9, 0);
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
      else if (MODRead >= 1000) // Button 3 clicked MISC
      {
        //1, 0);
        //2, 0);
        //3, 1);
        //4, 0);
        //5, 0);
        //6, 0);
        //7, 0);
        //8, 0);
        //9, 0);
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
          if (KNOB1Read > (KNOB1Value + controlThresh) || KNOB1Read < (KNOB1Value - controlThresh))
          {
            KNOB1Value=KNOB1Read;
            KNOB1Read = (KNOB1Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_Filter_Frequency, KNOB1Read);
          }
          if (KNOB2Read > (KNOB2Value + controlThresh) || KNOB2Read < (KNOB2Value - controlThresh))
          {
            KNOB2Value=KNOB2Read;
            KNOB2Read = (KNOB2Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_Filter_Resonance, KNOB2Read);
          }
          if (KNOB3Read > (KNOB3Value + controlThresh) || KNOB3Read < (KNOB3Value - controlThresh))
          {
            KNOB3Value=KNOB3Read;
            KNOB3Read = (KNOB3Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_Filter_Mode, KNOB3Read);
          }
          if (KNOB4Read > (KNOB4Value + controlThresh) || KNOB4Read < (KNOB4Value - controlThresh))
          {
            KNOB4Value=KNOB4Read;
            KNOB4Read = (KNOB4Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_Filter_Attenuation, KNOB4Read);
          }
        }
      }

      else if (MODRead < 120 && MODRead >= 100) // Button 4 clicked
      {
        //1, 0);
        //2, 0);
        //3, 0);
        //4, 1);
        //5, 0);
        //6, 0);
        //7, 0);
        //8, 0);
        //9, 0);

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
          if (KNOB1Read > (KNOB1Value + controlThresh) || KNOB1Read < (KNOB1Value - controlThresh))
          {
            KNOB1Value=KNOB1Read;
            KNOB1Read = (KNOB1Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_LFO_Level1, KNOB1Read);
          }
          if (KNOB2Read > (KNOB2Value + controlThresh) || KNOB2Read < (KNOB2Value - controlThresh))
          {
            KNOB2Value=KNOB2Read;
            KNOB2Read = (KNOB2Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_LFO1, KNOB2Read);
          }
          if (KNOB3Read > (KNOB3Value + controlThresh) || KNOB3Read < (KNOB3Value - controlThresh))
          {
            KNOB3Value=KNOB3Read;
            KNOB3Read = (KNOB3Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_LFO1, KNOB3Read);
          }
          if (KNOB4Read > (KNOB4Value + controlThresh) || KNOB4Read < (KNOB4Value - controlThresh))
          {
            KNOB4Value=KNOB4Read;
            KNOB4Read = (KNOB4Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_LFO_Rate1, KNOB4Read);
          }
        }
      }
      else if (MODRead < 150 && MODRead >= 120) // Button 5 clicked
      {
        //1, 0);
        //2, 0);
        //3, 0);
        //4, 0);
        //5, 1);
        //6, 0);
        //7, 0);
        //8, 0);
        //9, 0);
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
          if (KNOB1Read > (KNOB1Value + controlThresh) || KNOB1Read < (KNOB1Value - controlThresh))
          {
            KNOB1Value=KNOB1Read;
            KNOB1Read = (KNOB1Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_LFO_Level2, KNOB1Read);
          }
          if (KNOB2Read > (KNOB2Value + controlThresh) || KNOB2Read < (KNOB2Value - controlThresh))
          {
            KNOB2Value=KNOB2Read;
            KNOB2Read = (KNOB2Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_LFO2, KNOB2Read);
          }
          if (KNOB3Read > (KNOB3Value + controlThresh) || KNOB3Read < (KNOB3Value - controlThresh))
          {
            KNOB3Value=KNOB3Read;
            KNOB3Read = (KNOB3Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_LFO2, KNOB3Read);
          }
          if (KNOB4Read > (KNOB4Value + controlThresh) || KNOB4Read < (KNOB4Value - controlThresh))
          {
            KNOB4Value=KNOB4Read;
            KNOB4Read = (KNOB4Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_LFO_Rate2, KNOB4Read);
          }
        }
      }
      else if (MODRead < 320 && MODRead >= 290) // Button 6 clicked
      {
        //1, 0);
        //2, 0);
        //3, 0);
        //4, 0);
        //5, 0);
        //6, 1);
        //7, 0);
        //8, 0);
        //9, 0);

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
          if (KNOB1Read > (KNOB1Value + controlThresh) || KNOB1Read < (KNOB1Value - controlThresh))
          {
            KNOB1Value=KNOB1Read;
            KNOB1Read = (KNOB1Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_Filter_Attack, KNOB1Read);
          }
          if (KNOB2Read > (KNOB2Value + controlThresh) || KNOB2Read < (KNOB2Value - controlThresh))
          {
            KNOB2Value=KNOB2Read;
            KNOB2Read = (KNOB2Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_Filter_Decay, KNOB2Read);
          }
          if (KNOB3Read > (KNOB3Value + controlThresh) || KNOB3Read < (KNOB3Value - controlThresh))
          {
            KNOB3Value=KNOB3Read;
            KNOB3Read = (KNOB3Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_Filter_Sustain, KNOB3Read);
          }
          if (KNOB4Read > (KNOB4Value + controlThresh) || KNOB4Read < (KNOB4Value - controlThresh))
          {
            KNOB4Value=KNOB4Read;
            KNOB4Read = (KNOB4Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_Filter_Release, KNOB4Read);
          }
        }
      }
      else if (MODRead < 190 && MODRead >= 165) // Button 7 clicked
      {
        //1, 0);
        //2, 0);
        //3, 0);
        //4, 0);
        //5, 0);
        //6, 0);
        //7, 1);
        //8, 0);
        //9, 0);
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
          if (KNOB1Read > (KNOB1Value + controlThresh) || KNOB1Read < (KNOB1Value - controlThresh))
          {
            KNOB1Value=KNOB1Read;
            KNOB1Read = (KNOB1Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_Attack, KNOB1Read);
          }
          if (KNOB2Read > (KNOB2Value + controlThresh) || KNOB2Read < (KNOB2Value - controlThresh))
          {
            KNOB2Value=KNOB2Read;
            KNOB2Read = (KNOB2Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_Decay, KNOB2Read);
          }
          if (KNOB3Read > (KNOB3Value + controlThresh) || KNOB3Read < (KNOB3Value - controlThresh))
          {
            KNOB3Value=KNOB3Read;
            KNOB3Read = (KNOB3Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_Sustain, KNOB3Read);
          }
          if (KNOB4Read > (KNOB4Value + controlThresh) || KNOB4Read < (KNOB4Value - controlThresh))
          {
            KNOB4Value=KNOB4Read;
            KNOB4Read = (KNOB4Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_Release, KNOB4Read);
          }
        }
      }
      else if (MODRead < 235 && MODRead >= 215) // Button 8 clicked
      {
        //1, 0);
        //2, 0);
        //3, 0);
        //4, 0);
        //5, 0);
        //6, 0);
        //7, 0);
        //8, 1);
        //9, 0);
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
          if (KNOB1Read > (KNOB1Value + controlThresh) || KNOB1Read < (KNOB1Value - controlThresh))
          {
            KNOB1Value=KNOB1Read;
            KNOB1Read = (KNOB1Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_Flanger_On, KNOB1Read);
          }
          if (KNOB2Read > (KNOB2Value + controlThresh) || KNOB2Read < (KNOB2Value - controlThresh))
          {
            KNOB2Value=KNOB2Read;
            KNOB2Read = (KNOB2Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_Flanger_Offset, KNOB2Read);
          }
          if (KNOB3Read > (KNOB3Value + controlThresh) || KNOB3Read < (KNOB3Value - controlThresh))
          {
            KNOB3Value=KNOB3Read;
            KNOB3Read = (KNOB3Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_Flanger_Depth, KNOB3Read);
          }
          if (KNOB4Read > (KNOB4Value + controlThresh) || KNOB4Read < (KNOB4Value - controlThresh))
          {
            KNOB4Value=KNOB4Read;
            KNOB4Read = (KNOB4Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_Flanger_Fine, KNOB4Read);
          }
        }
      }
      else if (MODRead < 490 && MODRead >= 465) // Button 9 clicked
      {
        //1, 0);
        //2, 0);
        //3, 0);
        //4, 0);
        //5, 0);
        //6, 0);
        //7, 0);
        //8, 0);
        //9, 1);
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
          if (KNOB1Read > (KNOB1Value + controlThresh) || KNOB1Read < (KNOB1Value - controlThresh))
          {
            KNOB1Value=KNOB1Read;
            KNOB1Read = (KNOB1Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_Volume, KNOB1Read);
          }
          if (KNOB2Read > (KNOB2Value + controlThresh) || KNOB2Read < (KNOB2Value - controlThresh))
          {
            KNOB2Value=KNOB2Read;
            KNOB2Read = (KNOB2Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_Pan, KNOB2Read);
          }
          if (KNOB3Read > (KNOB3Value + controlThresh) || KNOB3Read < (KNOB3Value - controlThresh))
          {
            KNOB3Value=KNOB3Read;
            KNOB3Read = (KNOB3Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_Noise, KNOB3Read);
          }
          if (KNOB4Read > (KNOB4Value + controlThresh) || KNOB4Read < (KNOB4Value - controlThresh))
          {
            KNOB4Value=KNOB4Read;
            KNOB4Read = (KNOB4Read >> 3);
            OnControlChange(SYNTH_MIDICHANNEL, CC_Flanger_Fine, KNOB4Read);
          }
        }
      }
    }
    else
    {
      //1, 0);
      //2, 0);
      //3, 0);
      //4, 0);
      //5, 0);
      //6, 0);
      //7, 0);
      //8, 0);
      //9, 0);
      lcd.clear();
    }

  }
}