//////////////////////////////////////////////////////////////////////
// Oscillator control functions
//////////////////////////////////////////////////////////////////////

// set SYNTH_DEBUG to enable debug logging (1=most,2=all messages)
#include "global_variables.h"
#include "LCD.h"

inline float noteToFreq(float note, uint8_t waveform)
{
  // Sets all notes as an offset of A4 (#69)
  if (portamentoOn)
    note = portamentoPos;
  if (waveform == 1)
    octCorr = octCorr1;
  else
    octCorr = octCorr2;
  return SYNTH_TUNING * pow(2, (note - 69) / 12. + pitchBend / pitchScale + octCorr);
}

inline void oscOn(Oscillator &osc, int8_t note, uint8_t velocity)
{
  float v = velocityOn ? velocity / 127. : 1;
  if (osc.note != note)
  {
    osc.wf1->frequency(noteToFreq(note + octave1 * 12, 1));
    osc.wf2->frequency(noteToFreq((float)(note + octave2 * 12) * (1.00 - detune), 2));

    notesAdd(notesOn, note);
    if (envOn && !osc.velocity)
      osc.env->noteOn();
    osc.wf1->amplitude(v * channelVolume * GAIN_OSC * currentLevel1);
    osc.wf2->amplitude(v * channelVolume * GAIN_OSC * currentLevel2);

    osc.velocity = velocity;
    osc.note = note;
  }
  else if (velocity > osc.velocity)
  {
    osc.wf1->amplitude(v * channelVolume * GAIN_OSC);
    osc.wf2->amplitude(v * channelVolume * GAIN_OSC);

    osc.velocity = velocity;
  }
}

inline void oscOff(Oscillator &osc)
{
  if (envOn)
    osc.env->noteOff();
  else
  {
    osc.wf1->amplitude(0);
    osc.wf2->amplitude(0);
  }
  notesDel(notesOn, osc.note);
  osc.note = -1;
  osc.velocity = 0;
}

inline void allOff()
{
  Oscillator *o = oscs, *end = oscs + NVOICES;
  do
  {
    oscOff(*o);
    o->wf1->amplitude(0);
    o->wf2->amplitude(0);

    o->env->noteOff();
  } while (++o < end);
  notesReset(notesOn);
}

//////////////////////////////////////////////////////////////////////
// Parameter control functions
//////////////////////////////////////////////////////////////////////

inline void updateFilterMode()
{
  Oscillator *o = oscs, *end = oscs + NVOICES;
  do
  {
    for (uint8_t fm = 0; fm < 3; ++fm)
    {
      if (fm == filtermode)
        o->mix->gain(fm, filtAtt);
      else
        o->mix->gain(fm, 0);
    }
  } while (++o < end);
}

inline void updateFilter()
{
  Oscillator *o = oscs, *end = oscs + NVOICES;
  do
  {
    o->filt->frequency(filtFreq);
    o->filt->resonance(filtReso);
    o->filt->octaveControl(filtOctave);
  } while (++o < end);
}

inline void updateEnvelope()
{
  Oscillator *o = oscs, *end = oscs + NVOICES;
  do
  {
    o->env->attack(envAttack);
    o->env->decay(envDecay);
    o->env->sustain(envSustain);
    o->env->release(envRelease);
  } while (++o < end);
}

inline void updateFiltEnvelope()
{
  filterEnvelope.attack(FiltAttack);
  filterEnvelope.decay(FiltDecay);
  filterEnvelope.sustain(FiltSustain);
  filterEnvelope.release(FiltRelease);
  SYNTH_SERIAL.println((String)FiltRelease);
}
inline void updateEnvelopeMode()
{
  float env = envOn ? 1 : 0;
  float noenv = envOn ? 0 : 1;
  for (uint8_t i = 0; i < 2; ++i)
  {
    // env
    envmixer1.gain(i, env);
    envmixer2.gain(i, env);
    envmixer3.gain(i, env);
    envmixer4.gain(i, env);
    // no env
    envmixer1.gain(i + 2, noenv);
    envmixer2.gain(i + 2, noenv);
    envmixer3.gain(i + 2, noenv);
    envmixer4.gain(i + 2, noenv);
  }
}

void updateFlanger()
{
  if (flangerOn)
  {
    //AudioNoInterrupts();
    flangerL.voices(flangerOffset, flangerDepth, flangerFreqCoarse + flangerFreqFine);
    flangerR.voices(flangerOffset, flangerDepth, flangerFreqCoarse + flangerFreqFine);
    //AudioInterrupts();
#if SYNTH_DEBUG > 1
    SYNTH_SERIAL.print("Flanger: offset=");
    SYNTH_SERIAL.print(flangerOffset);
    SYNTH_SERIAL.print(", depth=");
    SYNTH_SERIAL.print(flangerDepth);
    SYNTH_SERIAL.print(", freq=");
    SYNTH_SERIAL.println(flangerFreqCoarse + flangerFreqFine);
#endif
  }
  else
  {
    flangerL.voices(FLANGE_DELAY_PASSTHRU, 0, 0);
    flangerR.voices(FLANGE_DELAY_PASSTHRU, 0, 0);
  }
}

inline void updateWaveforms()
{
  if (currentProgram1 == WAVEFORM_PULSE || currentProgram2 == WAVEFORM_PULSE || currentProgram3 == WAVEFORM_PULSE)
    octCorr = 1;
  else
    octCorr = 0;

  Oscillator *o = oscs, *end = oscs + NVOICES;
  do
  {
    o->wf1->begin(progs[currentProgram1]);
    o->wf2->begin(progs[currentProgram2]);
  } while (++o < end);
}

inline void updatePitch()
{
  Oscillator *o = oscs, *end = oscs + NVOICES;
  do
  {
    if (o->note < 0)
      continue;
    o->wf1->frequency(noteToFreq(o->note + octave1 * 12, 1));
    o->wf2->frequency(noteToFreq(o->note + octave2 * 12, 2));
  } while (++o < end);
}

// LFO ////////////////////////////////
inline void updateLFO1()
{ // LFO Pitch Rate/Level
  LFO_Pitch.begin(LFO1_Level, LFO1_Rate, progs[currentLFOProgram1]);
}

inline void updateLFO2()
{ // LFO Filter Rate/Level
  LFO_Filter.begin(LFO2_Level, LFO2_Rate, progs[currentLFOProgram2]);
}
inline void updatePWM()
{ // LFO Filter Rate/Level
  LFO_PWM.amplitude(LFO3_Level);
  LFO_PWM.frequency(LFO_PWM_Freq);
}
inline void updateVolume()
{
  Oscillator *o = oscs, *end = oscs + NVOICES;
  float velocity;
  do
  {
    if (o->note < 0)
      continue;
    velocity = velocityOn ? o->velocity / 127. : 1;
    o->wf1->amplitude(velocity * channelVolume * GAIN_OSC * currentLevel1);
    o->wf2->amplitude(velocity * channelVolume * GAIN_OSC * currentLevel2);
  } while (++o < end);
}
inline void updateMasterVolume()
{
  sgtl5000_1.volume(channelVolume / 2.);
}
inline void updatePan()
{
  for (uint8_t i = 0; i < 4; ++i)
  {
    mixerL.gain(i, left);
    mixerR.gain(i, right);
  }
}
inline void updateNoise()
{ // Noise too loud, therefore up to 0.5 gain.
  noise1.amplitude(noise);
  //pink1.amplitude(0);
}
inline void updateChorus()
{
  //chorusL.begin(delaylineL,DELAY_LENGTH,nvoices);
  //chorusR.begin(delaylineR,DELAY_LENGTH,nvoices);
}
/*
inline void updateMasterVolume() {
  // read the volume knob
  float vol = (float) analogRead(MASTER_VOL) / 1280.0;
  if( fabs(vol-masterVolume) > 0.01) {
    masterVolume = vol;
    sgtl5000_1.volume(masterVolume);
#if SYNTH_DEBUG > 0
    SYNTH_SERIAL.print("Volume: ");
    SYNTH_SERIAL.println(vol);
#endif
  }
}
*/
inline void updatePolyMode()
{
  allOff();
  updateEnvelopeMode();
  updatePan();
}

inline void updatePortamento()
{
  if (portamentoDir == 0)
    return;
  if (oscs->note < 0)
  {
    portamentoDir = 0;
    return;
  }
  if (portamentoDir < 0)
  {
    portamentoPos -= portamentoStep;
    if (portamentoPos < oscs->note)
    {
      portamentoPos = oscs->note;
      portamentoDir = 0;
    }
  }
  else
  {
    portamentoPos += portamentoStep;
    if (portamentoPos > oscs->note)
    {
      portamentoPos = oscs->note;
      portamentoDir = 0;
    }
  }
  oscs->wf1->frequency(noteToFreq(portamentoPos, 1));
  oscs->wf2->frequency(noteToFreq(portamentoPos, 2));
}

void savePatch()
{
}

void nextPatch()
{
}

void prevPatch()
{
}

void resetAll()
{

  // Master
  polyOn = true;
  omniOn = false;
  velocityOn = true;
  portamentoOn = false;
  sustainPressed = false;

  filtermode = 0;
  channelVolume = 0.5;
  panorama = 0.5;
  //pulseWidth     = 0.5;
  pitchBend = 0;
  pitchScale = 1;
  octCorr1 = currentProgram1 == WAVEFORM_PULSE ? 1 : 0;
  octCorr2 = currentProgram2 == WAVEFORM_PULSE ? 1 : 0;

  noise = 0;

  //Update functions
  updateWaveforms();
  updatePolyMode();
  updateVolume();
  updateNoise();

  // LFOs
  LFO1_Level = 0.5; //Pitch
  LFO1_Rate = 0;
  updateLFO1();

  LFO2_Level = 0; //Filter
  LFO2_Rate = 0;
  updateLFO2();

  LFO3_Level = 0.5;
  LFO_PWM_Freq = 1; //PWM
  LFO_PWM.begin(LFO3_Level, LFO_PWM_Freq, WAVEFORM_SINE);

  //Filter envelope
  FiltAttack = 20;
  FiltDecay = 400;
  FiltSustain = 1;
  FiltRelease = 20;

  // filter
  filtFreq = 15000.;
  filtReso = 0.9;
  filtAtt = 1.;
  updateFilterMode();
  updateFilter();

  envOn = true;
  envAttack = 20;
  envDecay = 0;
  envSustain = 5;
  envRelease = 20;
  updateEnvelope();

  updateEnvelopeMode();

  // MIXERS INITIALIZING//
  noisemixer.gain(0, 0.3);
  noisemixer.gain(1, 0.3);

  pitchmixer.gain(0, 0.1);

  //Filter Mixer
  filtermixer1.gain(0, 0.5);
  filtermixer1.gain(1, 0.5);
  filtermixer2.gain(0, 0.5);
  filtermixer2.gain(1, 0.5);
  filtermixer3.gain(0, 0.5);
  filtermixer3.gain(1, 0.5);
  filtermixer4.gain(0, 0.5);
  filtermixer4.gain(1, 0.5);
  filtermixer5.gain(0, 0.5);
  filtermixer5.gain(1, 0.5);
  filtermixer6.gain(0, 0.5);
  filtermixer6.gain(1, 0.5);
  filtermixer7.gain(0, 0.5);
  filtermixer7.gain(1, 0.5);
  filtermixer8.gain(0, 0.5);
  filtermixer8.gain(1, 0.5);

  dcEnvelope.amplitude(1.0);

  // PWM Mixer
  pwmmixer1.gain(0, 0);
  pwmmixer2.gain(0, 0);

  // Waveform Mixers
  mixer9.gain(0, 0.5);
  mixer9.gain(1, 0.5);
  mixer9.gain(2, 0.5);

  mixer10.gain(0, 0.5);
  mixer10.gain(1, 0.5);
  mixer10.gain(2, 0.5);

  mixer11.gain(0, 0.5);
  mixer11.gain(1, 0.5);
  mixer11.gain(2, 0.5);

  mixer12.gain(0, 0.5);
  mixer12.gain(1, 0.5);
  mixer12.gain(2, 0.5);

  mixer13.gain(0, 0.5);
  mixer13.gain(1, 0.5);
  mixer13.gain(2, 0.5);

  mixer14.gain(0, 0.5);
  mixer14.gain(1, 0.5);
  mixer14.gain(2, 0.5);

  mixer15.gain(0, 0.5);
  mixer15.gain(1, 0.5);
  mixer15.gain(2, 0.5);

  // FX
  flangerOffset = DELAY_LENGTH / 4;
  flangerDepth = DELAY_LENGTH / 4;
  flangerFreqCoarse = 0;
  flangerFreqFine = .5;

  flangerL.begin(delaylineL, DELAY_LENGTH, flangerOffset, flangerDepth, flangerFreqFine);
  flangerR.begin(delaylineR, DELAY_LENGTH, flangerOffset, flangerDepth, flangerFreqFine);
  flangerOn = false;
  updateFlanger();
}
