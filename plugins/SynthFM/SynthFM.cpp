
#include "ExtendedPlugin.hpp"
#include "synthFM.h"

START_NAMESPACE_DISTRHO

// Wrapper for FM synth
// FIXME: MPE flag is a hack, does not differentiate between channel, same pitch bend range applied to all
// TODO: available options such as voice re-use for same note
// NOTE: parameters related to waveform are CPU intensive
class SynthFM : public ExtendedPlugin {
public:
  // Note: do not care with default values since we will sent all parameters upon init
  SynthFM() : ExtendedPlugin(kParameterCount, 0, 0) {
    synthFM_Voice_process_init(context_processor);
    synthFM_Voice_setSamplerate(context_processor, float_to_fix((float)getSampleRate() / 1000.0f));
  }

protected:
  // metadata
  const char *getLabel() const override { return "BotaniaSynthFM"; }
  const char *getDescription() const override {
    return "I do FM things.";
  }
  const char *getMaker() const override { return "jfrey"; }
  uint32_t getVersion() const override { return d_version(1,0,0); }
  int64_t getUniqueId() const override { 
    return d_cconst('B','S','F','M'); 
  }

  // params
  void initParameter (uint32_t index, Parameter& parameter) override {
    // retrieve upper level for ADSR
    // note: 3-steps to get multiple returns from vult (need type, call function, get values)
    synthFM_ADSR_getMaxValues_type adsr;
    synthFM_ADSR_getMaxValues(adsr);
    float maxA = fix_to_float(synthFM_ADSR_getMaxValues_ret_0(adsr)); 
    float maxD = fix_to_float(synthFM_ADSR_getMaxValues_ret_1(adsr)); 
    float maxS = fix_to_float(synthFM_ADSR_getMaxValues_ret_2(adsr)); 
    float maxR = fix_to_float(synthFM_ADSR_getMaxValues_ret_3(adsr)); 
    synthFM_ADSR_getMinValues_type adsrmin;
    synthFM_ADSR_getMinValues(adsrmin);
    float minA = fix_to_float(synthFM_ADSR_getMinValues_ret_0(adsrmin)); 
    float minD = fix_to_float(synthFM_ADSR_getMinValues_ret_1(adsrmin)); 
    float minS = fix_to_float(synthFM_ADSR_getMinValues_ret_2(adsrmin)); 
    float minR = fix_to_float(synthFM_ADSR_getMinValues_ret_3(adsrmin)); 
    // also max number of wavetable to know up to what we can morph
    float maxMorph = synthFM_Wavetable_getNbWavetables();
    // quite arbitrary, actually. Nyquist will limit in practice (?)
    float maxRatio = 30.0;

    switch (index) {
    case kChannelInput:
      parameter.hints = kParameterIsInteger | kParameterIsAutomatable;
      parameter.name = "Input MIDI channel";
      parameter.shortName = "in chan";
      parameter.symbol = "inchannel";
      parameter.unit = "channel";
      // using enum to explicit omni. 0 for omni and 16 channels
      parameter.enumValues.count = 17;
      parameter.enumValues.restrictedMode = true;
      {
        ParameterEnumerationValue* const values = new ParameterEnumerationValue[parameter.enumValues.count];
        parameter.enumValues.values = values;
        values[0].label = "omni (all)";
        values[0].value = 0;
        values[1].label = "1";
        values[1].value = 1;
        values[2].label = "2";
        values[2].value = 2;
        values[3].label = "3";
        values[3].value = 3;
        values[4].label = "4";
        values[4].value = 4;
        values[5].label = "5";
        values[5].value = 5;
        values[6].label = "6";
        values[6].value = 6;
        values[7].label = "7";
        values[7].value = 7;
        values[8].label = "8";
        values[8].value = 8;
        values[9].label = "9";
        values[9].value = 9;
        values[10].label = "10";
        values[10].value = 10;
        values[11].label = "11";
        values[11].value = 11;
        values[12].label = "12";
        values[12].value = 12;
        values[13].label = "13";
        values[13].value = 13;
        values[14].label = "14";
        values[14].value = 14;
        values[15].label = "15";
        values[15].value = 15;
        values[16].label = "16";
        values[16].value = 16;
      }
      // select default idx
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 16.0f;
      break;
    case kPitchBendRange:
      // TODO: use a dedicated param for master pitch bend and one for MPE pitch bend
      parameter.hints = kParameterIsInteger | kParameterIsAutomatable;
      parameter.name = "Pitch bend range";
      parameter.shortName = "PB range";
      parameter.symbol = "pitchbend";
      parameter.unit = "semitones";
      // NOTE: default would be 48 for MPE messages (channel 2 to 16) and 2 only for master pitch bend (channel 1)
      parameter.ranges.def = 2.0f;
      parameter.ranges.min = 1.0f;
      parameter.ranges.max = 96.0f;
      break;
    case kModulatorAttack:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Modulator attack";
      parameter.shortName = "Mod A";
      parameter.symbol = "modulatorattack";
      parameter.unit = "seconds";
      parameter.ranges.def = 0.01f;
      parameter.ranges.min = minA;
      parameter.ranges.max = maxA;
      break;
    case kModulatorDecay:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Modulator decay";
      parameter.shortName = "Mod D";
      parameter.symbol = "modulatordecay";
      parameter.unit = "seconds";
      parameter.ranges.def = 0.01f;
      parameter.ranges.min = minD;
      parameter.ranges.max = maxD;
      break;
    case kModulatorSustain:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Modulator sustain";
      parameter.shortName = "Mod S";
      parameter.symbol = "modulatorsustain";
      parameter.unit = "seconds";
      parameter.ranges.def = 0.5f;
      parameter.ranges.min = minS;
      parameter.ranges.max = maxS;
      break;
    case kModulatorRelease:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Modulator release";
      parameter.shortName = "Mod R";
      parameter.symbol = "modulatorrelease";
      parameter.unit = "seconds";
      parameter.ranges.def = 0.01f;
      parameter.ranges.min = minR;
      parameter.ranges.max = maxR;
      break;
    case kModulatorRatio:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Modulator ratio";
      parameter.shortName = "Mod ratio";
      parameter.symbol = "modulatorratio";
      parameter.unit = "ratio";
      parameter.ranges.def = 2.0f;
      parameter.ranges.min = -maxRatio;
      parameter.ranges.max = maxRatio;
      break;
    case kModulatorWavetable:
      // NOTE: marked automatable to be exposed in DAW, but CPU intensive to change
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Modulator wavetable";
      parameter.shortName = "Mod wavetbl";
      parameter.symbol = "modulatorwavetable";
      parameter.unit = "index";
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = maxMorph;
      break;
    case kModulatorWavetablePhase:
      // NOTE: marked automatable to be exposed in DAW, but CPU intensive to change
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Modulator wavetable phase";
      parameter.shortName = "Mod wavetbl phase";
      parameter.symbol = "modulatorphase";
      parameter.unit = "phase";
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;
    case kModulatorLevel:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Modulator level";
      parameter.shortName = "Mod lvl";
      parameter.symbol = "modulatorlevel";
      parameter.unit = "level";
      parameter.ranges.def = 0.1f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;
    case kModulatorMode:
      parameter.hints = kParameterIsAutomatable | kParameterIsInteger;
      parameter.name = "Modulator target";
      parameter.shortName = "Mod target";
      parameter.symbol = "modulatortarget";
      parameter.unit = "target";
      parameter.enumValues.count = 2;
      parameter.enumValues.restrictedMode = true;
      {
        ParameterEnumerationValue* const values = new ParameterEnumerationValue[2];
        parameter.enumValues.values = values;
        values[0].label = "Phase";;
        values[0].value = 0;
        values[1].label = "Level";
        values[1].value = 1;
      }
      // select default idx
      parameter.ranges.def = 0.0f;
      break;
    case kModulatorShift:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Modulator phase shift";
      parameter.shortName = "Mod phase shift";
      parameter.symbol = "modulatorshift";
      parameter.unit = "shift";
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;
    case kModulatorFeedback:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Modulator feedback";
      parameter.shortName = "Mod fbk";
      parameter.symbol = "modulatorfeedback";
      parameter.unit = "feedback";
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;

    case kCarrierAttack:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Carrier attack";
      parameter.shortName = "Car A";
      parameter.symbol = "carrierattack";
      parameter.unit = "seconds";
      parameter.ranges.def = 0.01f;
      parameter.ranges.min = minA;
      parameter.ranges.max = maxA;
      break;
    case kCarrierDecay:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Carrier decay";
      parameter.shortName = "Car D";
      parameter.symbol = "carrierdecay";
      parameter.unit = "seconds";
      parameter.ranges.def = 0.01f;
      parameter.ranges.min = minD;
      parameter.ranges.max = maxD;
      break;
    case kCarrierSustain:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Carrier sustain";
      parameter.shortName = "Car S";
      parameter.symbol = "carriersustain";
      parameter.unit = "seconds";
      parameter.ranges.def = 0.5f;
      parameter.ranges.min = minS;
      parameter.ranges.max = maxS;
      break;
    case kCarrierRelease:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Carrier release";
      parameter.shortName = "Car R";
      parameter.symbol = "carrierrelease";
      parameter.unit = "seconds";
      parameter.ranges.def = 0.01f;
      parameter.ranges.min = minR;
      parameter.ranges.max = maxR;
      break;
    case kCarrierRatio:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Carrier ratio";
      parameter.shortName = "Car ratio";
      parameter.symbol = "carrierratio";
      parameter.unit = "ratio";
      parameter.ranges.def = 1.0f;
      parameter.ranges.min = -maxRatio;
      parameter.ranges.max = maxRatio;
      break;
    case kCarrierWavetable:
      // NOTE: marked automatable to be exposed in DAW, but CPU intensive to change
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Carrier wavetable";
      parameter.shortName = "Car wavetbl";
      parameter.symbol = "carrierwavetable";
      parameter.unit = "index";
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = maxMorph;
      break;
    case kCarrierWavetablePhase:
      // NOTE: marked automatable to be exposed in DAW, but CPU intensive to change
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Carrier wavetable phase";
      parameter.shortName = "Car wavetbl phase";
      parameter.symbol = "carrierphase";
      parameter.unit = "phase";
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;

    default:
      break;
    }
    
    // effectively set parameter
    setParameterValue(index, parameter.ranges.def);
  }

  float getParameterValue(uint32_t index) const override {
    switch (index) {

    case kChannelInput:
      return channelInput;
    case kPitchBendRange:
      return pitchBendRange;
    case kModulatorAttack:
      return modulatorAttack;
    case kModulatorDecay:
      return modulatorDecay;
    case kModulatorSustain:
      return modulatorSustain;
    case kModulatorRelease:
      return modulatorRelease;
    case kModulatorRatio:
      return modulatorRatio;
    case kModulatorWavetable:
      return modulatorWavetable;
    case kModulatorWavetablePhase:
      return modulatorWavetablePhase;
    case kModulatorLevel:
      return modulatorLevel;
    case kModulatorMode:
      return modulatorMode;
    case kModulatorShift:
      return modulatorShift;
    case kModulatorFeedback:
      return modulatorFeedback;

    case kCarrierAttack:
      return carrierAttack;
    case kCarrierDecay:
      return carrierDecay;
    case kCarrierSustain:
      return carrierSustain;
    case kCarrierRelease:
      return carrierRelease;
    case kCarrierRatio:
      return carrierRatio;
    case kCarrierWavetable:
      return carrierWavetable;
    case kCarrierWavetablePhase:
      return carrierWavetablePhase;

    default:
      return 0.0;
    }
  }
  
  void setParameterValue(uint32_t index, float value) override {
    // FIXME: check up to which point function is repeatedly called from host even when value does not change
    switch (index) {
    case kChannelInput:
      channelInput = value;
      break;
    case kPitchBendRange:
      pitchBendRange = value;
      break;
    case kModulatorAttack:
      modulatorAttack = value;
      // TODO: sadly no setter for each one, not very effective
      synthFM_Voice_synthSetModulatorADSR(context_processor, float_to_fix(modulatorAttack), float_to_fix(modulatorDecay), float_to_fix(modulatorSustain), float_to_fix(modulatorRelease));
      break;
    case kModulatorDecay:
      modulatorDecay = value;
      synthFM_Voice_synthSetModulatorADSR(context_processor, float_to_fix(modulatorAttack), float_to_fix(modulatorDecay), float_to_fix(modulatorSustain), float_to_fix(modulatorRelease));
      break;
    case kModulatorSustain:
      modulatorSustain = value;
      synthFM_Voice_synthSetModulatorADSR(context_processor, float_to_fix(modulatorAttack), float_to_fix(modulatorDecay), float_to_fix(modulatorSustain), float_to_fix(modulatorRelease));
      break;
    case kModulatorRelease:
      modulatorRelease = value;
      synthFM_Voice_synthSetModulatorADSR(context_processor, float_to_fix(modulatorAttack), float_to_fix(modulatorDecay), float_to_fix(modulatorSustain), float_to_fix(modulatorRelease));
      break;
    case kModulatorRatio:
      synthFM_Voice_synthSetModulatorRatio(context_processor, float_to_fix(value));
      modulatorRatio = value;
      break;
    case kModulatorWavetable:
      synthFM_Voice_synthSetModulatorWavetable(context_processor, float_to_fix(value));
      modulatorWavetable = value;
      break;
    case kModulatorWavetablePhase:
      synthFM_Voice_synthSetModulatorWavetablePhase(context_processor, float_to_fix(value));
      modulatorWavetablePhase = value;
      break;
    case kModulatorLevel:
      synthFM_Voice_synthSetModulatorLevel(context_processor, float_to_fix(value));
      modulatorLevel = value;
      break;
    case kModulatorMode:
      // 0 is phase above
      if (value > 0.5f) {
        synthFM_Voice_synthSetModulatorTargetLevel(context_processor, true);
      }
      else {
        synthFM_Voice_synthSetModulatorTargetLevel(context_processor, false);
      }
      modulatorMode = value;
      break;
    case kModulatorShift:
      synthFM_Voice_synthSetModulatorPhaseShift(context_processor, float_to_fix(value));
      modulatorShift = value;
      break;
    case kModulatorFeedback:
      synthFM_Voice_synthSetModulatorFeedback(context_processor, float_to_fix(value));
      modulatorFeedback = value;
      break;

    case kCarrierAttack:
      carrierAttack = value;
      synthFM_Voice_synthSetCarrierADSR(context_processor, float_to_fix(carrierAttack), float_to_fix(carrierDecay), float_to_fix(carrierSustain), float_to_fix(carrierRelease));
      break;
    case kCarrierDecay:
      carrierDecay = value;
      synthFM_Voice_synthSetCarrierADSR(context_processor, float_to_fix(carrierAttack), float_to_fix(carrierDecay), float_to_fix(carrierSustain), float_to_fix(carrierRelease));
      break;
    case kCarrierSustain:
      carrierSustain = value;
      synthFM_Voice_synthSetCarrierADSR(context_processor, float_to_fix(carrierAttack), float_to_fix(carrierDecay), float_to_fix(carrierSustain), float_to_fix(carrierRelease));
      break;
    case kCarrierRelease:
      carrierRelease = value;
      synthFM_Voice_synthSetCarrierADSR(context_processor, float_to_fix(carrierAttack), float_to_fix(carrierDecay), float_to_fix(carrierSustain), float_to_fix(carrierRelease));
      break;
    case kCarrierRatio:
      synthFM_Voice_synthSetCarrierRatio(context_processor, float_to_fix(value));
      carrierRatio = value;
      break;
    case kCarrierWavetable:
      synthFM_Voice_synthSetCarrierWavetable(context_processor, float_to_fix(value));
      carrierWavetable = value;
      break;
    case kCarrierWavetablePhase:
      synthFM_Voice_synthSetCarrierWavetablePhase(context_processor, float_to_fix(value));
      carrierWavetablePhase = value;
      break;

    default:
      break;
    }
  }

  // callbacks for processing MIDI
  void noteOn(uint8_t note, uint8_t velocity, uint8_t channel, uint32_t) {
    // filter event depending on selected channel
    if (channelInput == 0 or channelInput - 1 == channel) {
      synthFM_Voice_noteOn(context_processor, note, velocity, channel);
    }
  }

  void noteOff(uint8_t note, uint8_t channel, uint32_t) {
    // filter event depending on selected channel
    if (channelInput == 0 or channelInput - 1 == channel) {
      synthFM_Voice_noteOff(context_processor, note, channel);
    }
  }

  void pitchbend(uint8_t channel, float ratio, uint32_t) {
    // filter event depending on selected channel
    if (channelInput == 0 or channelInput - 1 == channel) {
      synthFM_Voice_synthPitchBend(context_processor, float_to_fix(ratio * pitchBendRange));
    }
  }

    void sustain(uint8_t channel, bool flag, uint32_t) {
    // filter event depending on selected channel
    if (channelInput == 0 or channelInput - 1 == channel) {
      synthFM_Voice_synthSetSustain(context_processor, flag);
    }
  }

  void process(uint32_t chunkSize, uint32_t) {
    synthFM_Voice_process_bufferTo(context_processor, chunkSize, buffOut);
  }

  // Optional callback to inform synth about a sample rate change on the plugin side.
  void sampleRateChanged(double newSampleRate) override
  {
    synthFM_Voice_setSamplerate(context_processor, float_to_fix((float)newSampleRate / 1000.0f));
  }

private:
  synthFM_Voice_process_type context_processor;

  // parameters
  int channelInput;
  int pitchBendRange;

  // giving some default values to ADSR here to avoid calls with uninitialized variable upon parameters' init
  float modulatorAttack = 0.01;
  float modulatorDecay = 0.01;
  float modulatorSustain = 0.5;
  float modulatorRelease = 0.01;
  float modulatorRatio;
  float modulatorWavetable;
  float modulatorWavetablePhase;
  float modulatorLevel;
  float modulatorMode;
  float modulatorShift;
  float modulatorFeedback;

  float carrierAttack = 0.01;
  float carrierDecay = 0.01;
  float carrierSustain = 0.5;
  float carrierRelease = 0.01;
  float carrierRatio;
  float carrierWavetable;
  float carrierWavetablePhase;


  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthFM);
};

Plugin *createPlugin() { return new SynthFM(); }

END_NAMESPACE_DISTRHO
