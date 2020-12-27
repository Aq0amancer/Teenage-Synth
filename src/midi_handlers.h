//////////////////////////////////////////////////////////////////////
// MIDI handlers
//////////////////////////////////////////////////////////////////////
#include "global_variables.h"

Oscillator *OnNoteOffReal(uint8_t channel, uint8_t note, uint8_t velocity, bool ignoreSustain)
{
  if (!omniOn && channel != SYNTH_MIDICHANNEL)
    return 0;

#if 0 //#ifdef SYNTH_DEBUG
  SYNTH_SERIAL.println("NoteOff");
#endif
  int8_t lastNote = notesDel(notesPressed, note);
  filterEnvelope.noteOff();

  if (sustainPressed && !ignoreSustain)
    return 0;

  Oscillator *o = oscs;
  if (portamentoOn)
  {
    if (o->note == note)
    {
      if (lastNote != -1)
      {
        notesDel(notesOn, note);
        if (portamentoTime == 0)
        {
          portamentoPos = lastNote;
          portamentoDir = 0;
        }
        else
        {
          portamentoDir = lastNote > portamentoPos ? 1 : -1;
          portamentoStep = fabs(lastNote - portamentoPos) / (portamentoTime);
        }
        oscOn(*o, lastNote, velocity);
      }
      else
      {
        oscOff(*o);
        portamentoPos = -1;
        portamentoDir = 0;
      }
    }
    if (oscs->note == note)
    {
      if (lastNote != -1)
      {
        notesDel(notesOn, o->note);
        oscOn(*o, lastNote, velocity);
      }
      else
      {
        oscOff(*o);
      }
    }
  }
  else if (polyOn)
  {
    Oscillator *end = oscs + NVOICES;
    do
    {
      if (o->note == note)
        break;
    } while (++o < end);
    if (o == end)
      return 0;
    oscOff(*o);
  }
  else
  {
    if (oscs->note == note)
    {
      if (lastNote != -1)
      {
        notesDel(notesOn, o->note);
        oscOn(*o, lastNote, velocity);
      }
      else
      {
        oscOff(*o);
      }
    }
  }

  return o;
}

void OnNoteOn(uint8_t channel, uint8_t note, uint8_t velocity)
{
  if (!omniOn && channel != SYNTH_MIDICHANNEL)
    return;

#if SYNTH_DEBUG > 1
  SYNTH_SERIAL.println("NoteOn");
#endif
  filterEnvelope.noteOn();

  notesAdd(notesPressed, note);

  Oscillator *o = oscs;
  Oscillator *curOsc = 0, *end = oscs + NVOICES;
  if (sustainPressed && notesFind(notesOn, note))
  {
    do
    {
      if (o->note == note)
      {
        curOsc = o;
        break;
      }
    } while (++o < end);
  }
  for (o = oscs; o < end && !curOsc; ++o)
  {
    if (o->note < 0)
    {
      curOsc = o;
      break;
    }
  }
  if (!curOsc && *notesOn != -1)
  {
#if SYNTH_DEBUG > 0
    SYNTH_SERIAL.println("Stealing voice");
#endif
    curOsc = OnNoteOffReal(channel, *notesOn, velocity, true);
  }
  if (!curOsc)
    return;
  oscOn(*curOsc, note, velocity);

  return;
}
inline void OnNoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
{
  OnNoteOffReal(channel, note, velocity, true);
}

void OnAfterTouchPoly(uint8_t channel, uint8_t note, uint8_t value)
{
#if SYNTH_DEBUG > 0
  SYNTH_SERIAL.print("AfterTouchPoly: channel ");
  SYNTH_SERIAL.print(channel);
  SYNTH_SERIAL.print(", note ");
  SYNTH_SERIAL.print(note);
  SYNTH_SERIAL.print(", value ");
  SYNTH_SERIAL.println(value);
#endif
}

/////////////////////////////////// SYNTH MASTER CONTROL ///////////////////////////////////////////////////////////////////////////////////////////

void OnControlChange(uint8_t channel, uint8_t control, uint8_t value)
{
  if (!omniOn && channel != SYNTH_MIDICHANNEL)
    return;

  switch (control)
  {

    //////////////////////// WAVEFORM OSC 1 /////////////////////////////////////////////////////////////////
  case CC_Level1:
  {
    float volume1 = value / 127.;
    waveform1.amplitude(volume1);
    waveform3.amplitude(volume1);
    waveform5.amplitude(volume1);
    waveform7.amplitude(volume1);
    waveform9.amplitude(volume1);
    waveform11.amplitude(volume1);
    waveform13.amplitude(volume1);
    waveform15.amplitude(volume1);
    break;
  }
  case CC_OSC1: // Waveform OSC1
  {
    if (value <= 18)
      currentProgram1 = 0;
    else if (value > 18 && value <= 36)
      currentProgram1 = 1;
    else if (value > 36 && value <= 54)
      currentProgram1 = 2;
    else if (value > 54 && value <= 72)
      currentProgram1 = 3;
    else if (value > 72 && value <= 90)
      currentProgram1 = 4;
    else if (value > 90 && value <= 108)
      currentProgram1 = 5;
    else if (value > 108 && value <= 127)
      currentProgram1 = 6;

    updateWaveforms();
    sprintf(buffer_secondrow, "Waveform: %s", waveforms[currentProgram1]);
    printLCD("Oscillator 1", buffer_secondrow);
    break;
  }
#if SYNTH_DEBUG > 1
    SYNTH_SERIAL.print("Waveform 1 change: channel ");
    SYNTH_SERIAL.print(channel);
    SYNTH_SERIAL.print(", program ");
    SYNTH_SERIAL.println((String)progs[currentProgram1]);
#endif
    break;

  case CC_Octave1: // Octave OSC 1
  {
    octave1 = map(value, 0, 127, 3, -3); // Map any value between the 2 octaves -12 to 12
    sprintf(buffer_secondrow, " Octave %d", octave1);
    printLCD("Oscillator 1", buffer_secondrow);
    break;
  }
  case CC_PWM1: // Pulse Width Ammount OSC 1
  {
    gain_pwm1 = 1. - value / 127.;
    pwmmixer1.gain(0, gain_pwm1);
    sprintf(buffer_secondrow, "PWM Gain %f", gain_pwm1);
    printLCD("Oscillator 1", buffer_secondrow);
    break;
  }
    //////////////////////////////// WAVEFORM OSC 2 /////////////////////////////////////////////
  case CC_Level2:
  {
    float volume2 = value / 127.;
    waveform2.amplitude(volume2);
    waveform4.amplitude(volume2);
    waveform6.amplitude(volume2);
    waveform8.amplitude(volume2);
    waveform10.amplitude(volume2);
    waveform12.amplitude(volume2);
    waveform14.amplitude(volume2);
    waveform16.amplitude(volume2);
    break;
  }
  case CC_OSC2: // Waveform1 OSC2
  {
    if (value <= 18)
      currentProgram2 = 0;
    else if (value > 18 && value <= 36)
      currentProgram2 = 1;
    else if (value > 36 && value <= 54)
      currentProgram2 = 2;
    else if (value > 54 && value <= 72)
      currentProgram2 = 3;
    else if (value > 72 && value <= 90)
      currentProgram1 = 4;
    else if (value > 90 && value <= 108)
      currentProgram2 = 5;
    else if (value > 108 && value <= 127)
      currentProgram2 = 6;
    updateWaveforms();
    // Print LCD
    sprintf(buffer_secondrow, "Waveform: %s", waveforms[currentProgram2]);
    printLCD("Oscillator 2", buffer_secondrow);
    break;
  }
#if SYNTH_DEBUG > 1
    SYNTH_SERIAL.print("Waveform 2 change: channel ");
    SYNTH_SERIAL.print(channel);
    SYNTH_SERIAL.print(", program ");
    SYNTH_SERIAL.println((String)progs[currentProgram2]);
#endif
    break;

  case CC_Octave2: // Octave OSC2
  {
    octave2 = map(value, 0, 127, 3, -3); // Map any value between the 2 octaves -12 to 12
    sprintf(buffer_secondrow, " Octave: %d", octave2);
    printLCD("Oscillator 2", buffer_secondrow);
    break;
  }
  case CC_PWM2: // Pulse Width OSC2
  {
    gain_pwm2 = 1.00 - value / 127.;
    pwmmixer2.gain(0, gain_pwm2);
    sprintf(buffer_secondrow, "PWM Gain: %f", gain_pwm2);
    printLCD("Oscillator 2", buffer_secondrow);
    break;
  }
    //////////////////////// LFO 1: Pitch /////////////////////////////////////////////////////////////////

  case CC_LFO_Level1: // LFO1 level
  {
    LFO1_Level = 0.5 - value / 256.;
    updateLFO1();
    sprintf(buffer_secondrow, "Level: %f", LFO1_Level);
    printLCD("Pitch LFO", buffer_secondrow);
    break;
  }
  case CC_LFO1: // LFO1 waveform 1
  {
    if (value <= 18)
      currentLFOProgram1 = 0;
    else if (value > 18 && value <= 36)
      currentLFOProgram1 = 1;
    else if (value > 36 && value <= 54)
      currentLFOProgram1 = 2;
    else if (value > 54 && value <= 72)
      currentLFOProgram1 = 3;
    else if (value > 72 && value <= 90)
      currentLFOProgram1 = 4;
    else if (value > 90 && value <= 108)
      currentLFOProgram1 = 5;
    else if (value > 108 && value <= 127)
      currentLFOProgram1 = 6;
    updateLFO1();
    sprintf(buffer_secondrow, "Waveform: %s", waveforms[currentLFOProgram1]);
    printLCD("Pitch LFO", buffer_secondrow);

    break;
  }
  case CC_LFO_Rate1: // LFO1 Rate
  {
    LFO1_Rate = 8.0 - value / 16.; // Rate 1 - 8 Hz
    updateLFO1();
    sprintf(buffer_secondrow, "Rate: %f Hz", LFO1_Rate);
    printLCD("Pitch LFO", buffer_secondrow);
    break;
  }
    //////////////////// LFO 2: Filter ///////////////////////////////////////////

  case CC_LFO_Level2: // LFO2 Level
  {
    LFO2_Level = 0.5 - value / 256.;
    //printLCD("Filter LFO", "Level", (char)LFO2_Level, "");
    updateLFO2();
    break;
  }
  case CC_LFO2: // LFO2 waveform 1
  {
    if (value > 0)
    {
      currentLFOProgram2 = 0;
      updateLFO2();
    }
    break;
  }
  case CC_LFO_Rate2: // LFO2 Rate
  {
    LFO2_Rate = 16.0 - value / 8.; // Rate 1 - 4 Hz
    updateLFO2();
    //printLCD("Filter LFO", "Rate", (char)LFO2_Rate, "Hz");
    break;
  }
    /////////////// MASTER ////////////////////////////////////////////

  case CC_Portamento: // portamento time FIX FIX FIX FIX
  {
    //printLCD("Master", "Glide", (char)value, "");
    if (value < 63)
    {
      portamentoOn = true;
      float portamentoRange = portamentoStep * portamentoTime;
      portamentoTime = value * 50;
      portamentoStep = portamentoRange / portamentoTime;
      if (oscs->note != -1)
        portamentoPos = oscs->note;
      portamentoOn = false;
    }
    else
    {
      portamentoOn = false;
    }
    break;
  }
  case CC_Volume: // volume
  {
    if (value == 127)
    {
      allOff();
    }
    channelVolume = 1 - value / 127.;
    updateMasterVolume();
    //printLCD("Master", "Volume", (char)channelVolume, "");
    break;
  }
  case CC_Pan: // PAN
  {
    if (value > 64)
    {
      left = 1.00;
      right = (127 - value) / 64.;
      //printLCD("Master", "Pan", (char)right, "L");
    }
    else
    {
      right = 1.00;
      left = value / 64.;
      //printLCD("Master", "Pan", (char)left, "R");
    }
    updatePan();
    break;
  }
  case CC_Noise: // White Noise
  {
    noise = (127 - value) / 127;
    updateNoise();
    //char noise_value[4];
    //sprintf(noise_value, "%f", noise);
    //printLCD("Master", "Noise", (char)noise, "");
    break;
  }
    //////////////// AMP Envelope/////////////////////////////////////////

  case CC_Attack: // attack
  {
    envAttack = envelopeMax - value * envelopeMax / 127.;
    updateEnvelope();
    //printLCD("Amp", "Attack", (char)envAttack, "ms");
    break;
  }
  case CC_Decay: // decay
  {
    envDecay = envelopeMax - value * envelopeMax / 127.;
    updateEnvelope();
    //printLCD("Amp", "Decay", (char)envDecay, "ms");
    break;
  }
  case CC_Sustain: // sustain
  {
    envSustain = 1.00 - value / 127.;
    updateEnvelope();
    //printLCD("Amp", "Sustain", (char)envSustain, "");
    break;
  }
  case CC_Release: // release
  {
    envRelease = envelopeMax - value * envelopeMax / 127.;
    updateEnvelope();
    //printLCD("Amp", "Release", (char)envRelease, "ms");
    break;
  }
    //////////////// FILTER Envelope/////////////////////////////////////////

  case CC_Filter_Attack: // attack
  {
    FiltAttack = envelopeMax - value * envelopeMax / 127.;
    updateFiltEnvelope();
    //printLCD("Filter", "Attack", (char)FiltAttack, "ms");
    break;
  }
  case CC_Filter_Decay: // decay
  {
    FiltDecay = envelopeMax - value * envelopeMax / 127.;
    updateFiltEnvelope();
    //printLCD("Filter", "Decay", (char)FiltDecay, "ms");
    break;
  }
  case CC_Filter_Sustain: // sustain
  {
    FiltSustain = 1.00 - value / 127.;
    updateFiltEnvelope();
    //printLCD("Filter", "Sustain", (char)FiltSustain, "");
    break;
  }
  case CC_Filter_Release: // release
  {
    FiltRelease = envelopeMax - value * envelopeMax / 127.;
    updateFiltEnvelope();
    //printLCD("Filter", "Release", (char)FiltRelease, "ms");
    break;
  }
    //////////////// FILTER ///////////////////////////////////////////

  case CC_Filter_Frequency: // filter frequency
  {
    filtFreq = FILTERFREQS[value];

    if (filtFreq > 2500)
    {
      filtOctave = 2.0f; //Allows more accurate filter cutoff
    }
    else if (filtFreq < 60)
    {
      filtOctave = 7.0f; //Allows deeper bass
    }
    else
    {
      filtOctave = 2.0f + ((2560 - filtFreq) / 500); //In between
    }
    updateFilter();
    //printLCD("Filter", "Cutoff", (char)filtFreq, "Hz");
    break;
  }
  case CC_Filter_Resonance: // filter resonance
  {
    filtReso = 5.0 - value * 4.1 / 127. + 0.9;
    updateFilter();
    //printLCD("Filter", "Resonance", (char)filtReso, "");
    break;
  }
  case CC_Filter_Attenuation: // filter attenuation
  {
    filtAtt = 1.00 - value / 127.;
    updateFilterMode();
    //printLCD("Filter", "Level", (char)filtAtt, "");
    break;
  }
  case CC_Filter_Mode: // filter mode
  {
    filtermode = map(value, 0, 127, 0, 2);
    updateFilterMode();
    switch (filtermode)
    {
    case 0:
      //printLCD("Filter", "Mode", "Low", "");
      break;
    case 1:
      //printLCD("Filter", "Mode", "Band", "");
      break;
    case 2:
      //printLCD("Filter", "Mode", "High", "");
      break;
    }
    break;
  }
    ////////////////////////////////////////////////////////////////////////////

  case CC_PWM_Rate:
  {
    LFO_PWM_Freq = 2.00 - value / 64.;
    updatePWM();
    //printLCD("Osc Control", "PWM Rate", (char)LFO_PWM_Freq, "Hz");
    break;
  }
  case CC_Envelope_Mode: // envelope mode
  {
    allOff();
    envOn = !envOn;
    updateEnvelopeMode();
    break;
  }
    ////////////// FLANGER ////////////////////////////////////////////////////////////////

  case CC_Flanger_On: // flanger toggle button
  {
    if (value > 0)
      flangerOn = true;
    else
      flangerOn = false;
    updateFlanger();
    break;
  }
  case CC_Flanger_Offset: // flanger offset
  {
    flangerOffset = DELAY_LENGTH / (4 * map(value, 0, 127, 4, 1));
    updateFlanger();
    //printLCD("Flanger", "Offset", (char)flangerOffset, "");
    break;
  }
  case CC_Flanger_Depth: // flanger depth
  {
    flangerDepth = DELAY_LENGTH / (4 * map(value, 0, 127, 4, 1));
    updateFlanger();
    //printLCD("Flanger", "Depth", (char)flangerDepth, "");
    break;
  }
  case CC_Flanger_Fine: // flanger coarse frequency
  {
    flangerFreqCoarse = value / 127.;
    updateFlanger();
    //printLCD("Flanger", "Coarse", (char)flangerFreqCoarse, "");
    break;
  }
    ///////////// UNDEFINED ////////////////////////////////////////////////////////////////////
  case CC_Oscmix:
  {
    if (value > 64)
    {
      currentLevel1 = 1.0;
      currentLevel2 = (127. - value) / 64.;
    }
    else
    {
      currentLevel2 = 1.0;
      currentLevel1 = value / 64.;
    }
    updateVolume();
    break;
  }
  case CC_Detune:
  {
    detune = value / 20000.;
    //printLCD("Osc Control", "Detune", (char)float(detune * 100.), "%");
    break;
  }
  case CC_Sustain_Pedal: // sustain/damper pedal
  {
    if (value > 63)
      sustainPressed = false;
    else
    {
      sustainPressed = false;
      Oscillator *o = oscs, *end = oscs + NVOICES;
      do
      {
        if (o->note != -1 && !notesFind(notesPressed, o->note))
          oscOff(*o);
      } while (++o < end);
    }
    break;
  }
  case 84: // portamento control (start note)
  {
    portamentoPos = value;
    break;
  case 121: // controller reset
    resetAll();
    break;
  }
  default:
#if SYNTH_DEBUG > 0
    SYNTH_SERIAL.print("Unhandled Control Change: channel ");
    SYNTH_SERIAL.print(channel);
    SYNTH_SERIAL.print(", control ");
    SYNTH_SERIAL.print(control);
    SYNTH_SERIAL.print(", value ");
    SYNTH_SERIAL.println(value);
#endif
    break;
  }
#if SYNTH_DEBUG > 1 //0
  SYNTH_SERIAL.print("Control Change: channel ");
  SYNTH_SERIAL.print(channel);
  SYNTH_SERIAL.print(", control ");
  SYNTH_SERIAL.print(control);
  SYNTH_SERIAL.print(", value ");
  SYNTH_SERIAL.println(value);
#endif
}

////////////////////////////////// END CONTROL CHANGE ///////////////////////////////////////////////////

void OnPitchChange(uint8_t channel, int pitch)
{
  if (!omniOn && channel != SYNTH_MIDICHANNEL)
    return;

#if SYNTH_DEBUG > 1
  SYNTH_SERIAL.print("PitchChange: channel ");
  SYNTH_SERIAL.print(channel);
  SYNTH_SERIAL.print(", pitch ");
  SYNTH_SERIAL.println(pitch);
#endif

#ifdef USB_MIDI
  if (pitch == 8192)
    pitchBend = 0;
  else
    pitchBend = (pitch - 8192) / 8192.;
#else
  pitchBend = pitch / 8192.;
#endif

  updatePitch();
}

//////////////////////////////////////////// A F T E R T O U C H ////////////////////

void OnAfterTouch(uint8_t channel, uint8_t pressure)
{
  if (!omniOn && channel != SYNTH_MIDICHANNEL)
    return;

#if SYNTH_DEBUG > 0
  SYNTH_SERIAL.print("AfterTouch: channel ");
  SYNTH_SERIAL.print(channel);
  SYNTH_SERIAL.print(", pressure ");
  SYNTH_SERIAL.println(pressure);
#endif
}

void OnSysEx(const uint8_t *data, uint16_t length, bool complete)
{
#if SYNTH_DEBUG > 0
  SYNTH_SERIAL.print("SysEx: length ");
  SYNTH_SERIAL.print(length);
  SYNTH_SERIAL.print(", complete ");
  SYNTH_SERIAL.println(complete);
#endif
}

void OnRealTimeSystem(uint8_t realtimebyte)
{
#if SYNTH_DEBUG > 0
  SYNTH_SERIAL.print("RealTimeSystem: ");
  SYNTH_SERIAL.println(realtimebyte);
#endif
}

void OnTimeCodeQFrame(uint16_t data)
{
#if SYNTH_DEBUG > 0
  SYNTH_SERIAL.print("TimeCodeQuarterFrame: ");
  SYNTH_SERIAL.println(data);
#endif
}
