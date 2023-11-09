
#include "DistrhoPlugin.hpp"
#include "synthFM.h"

START_NAMESPACE_DISTRHO

// chunk size to process audio
// to sync with vult code
#define BUFFER_SIZE 128

// Wrapper for FM synth
// TODO: available options such as voice re-use for same note
class SynthFM : public Plugin {
public:
  SynthFM() : Plugin(kParameterCount, 0, 0) {
    synthFM_Voice_process_init(context_processor);
  }

protected:
  // metadata
  const char *getLabel() const override { return "BotaniaSaturator"; }
  const char *getDescription() const override {
    return "I do FM things.";
  }
  const char *getMaker() const override { return "jfrey"; }
  const char *getLicense() const override { return "Custom"; }
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
    case kModulatorAttack:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Modulator attack";
      parameter.symbol = "seconds";
      parameter.ranges.def = 0.01f;
      parameter.ranges.min = minA;
      parameter.ranges.max = maxA;
      break;
    case kModulatorDecay:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Modulator decay";
      parameter.symbol = "seconds";
      parameter.ranges.def = 0.01f;
      parameter.ranges.min = minD;
      parameter.ranges.max = maxD;
      break;
    case kModulatorSustain:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Modulator sustain";
      parameter.symbol = "seconds";
      parameter.ranges.def = 0.5f;
      parameter.ranges.min = minS;
      parameter.ranges.max = maxS;
      break;
    case kModulatorRelease:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Modulator release";
      parameter.symbol = "seconds";
      parameter.ranges.def = 0.01f;
      parameter.ranges.min = minR;
      parameter.ranges.max = maxR;
      break;
    case kModulatorRatio:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Modulator ratio";
      parameter.symbol = "ratio";
      parameter.ranges.def = 2.0f;
      parameter.ranges.min = -maxRatio;
      parameter.ranges.max = maxRatio;
      break;
    case kModulatorWavetable:
      // FIXME: not automatable due to heavy computation at the moment upon change
      parameter.name = "Modulator wavetable";
      parameter.symbol = "index";
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = maxMorph;
      break;
    case kModulatorWavetablePhase:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Modulator wavetable phase";
      parameter.symbol = "phase";
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;
    case kModulatorLevel:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Modulator level";
      parameter.symbol = "level";
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;
    case kModulatorMode:
      parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
      parameter.name = "Modulator target";
      parameter.symbol = "target";
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
      break;
    case kModulatorShift:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Modulator phase shift";
      parameter.symbol = "shift";
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;
    case kModulatorFeedback:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Modulator feedback";
      parameter.symbol = "feedback";
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;

    case kCarrierAttack:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Carrier attack";
      parameter.symbol = "seconds";
      parameter.ranges.def = 0.01f;
      parameter.ranges.min = minA;
      parameter.ranges.max = maxA;
      break;
    case kCarrierDecay:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Carrier decay";
      parameter.symbol = "seconds";
      parameter.ranges.def = 0.01f;
      parameter.ranges.min = minD;
      parameter.ranges.max = maxD;
      break;
    case kCarrierSustain:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Carrier sustain";
      parameter.symbol = "seconds";
      parameter.ranges.def = 0.5f;
      parameter.ranges.min = minS;
      parameter.ranges.max = maxS;
      break;
    case kCarrierRelease:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Carrier release";
      parameter.symbol = "seconds";
      parameter.ranges.def = 0.01f;
      parameter.ranges.min = minR;
      parameter.ranges.max = maxR;
      break;
    case kCarrierRatio:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Carrier ratio";
      parameter.symbol = "ratio";
      parameter.ranges.def = 1.0f;
      parameter.ranges.min = -maxRatio;
      parameter.ranges.max = maxRatio;
      break;
    case kCarrierWavetable:
      // FIXME: not automatable due to heavy computation at the moment upon change
      parameter.name = "Carrier wavetable";
      parameter.symbol = "index";
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = maxMorph;
      break;
    case kCarrierWavetablePhase:
      parameter.name = "Modulator wavetable phase";
      parameter.symbol = "phase";
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;

    default:
      break;
    }

    // effectively set parameter
    // TODO: check if needed
    //setParameterValue(index, parameter.ranges.def);

  }

  float getParameterValue(uint32_t index) const override {
    switch (index) {

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
        synthFM_Voice_synthSetModulatorTargetLevel(context_processor, false);
      }
      else {
        synthFM_Voice_synthSetModulatorTargetLevel(context_processor, true);
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

  void run(const float**, float** outputs, uint32_t frames,
             const MidiEvent* midiEvents, uint32_t midiEventCount) override {

    // deal with MIDI input
    // FIXME: take into consideration the timestamp and place before correct chunk
    for (uint32_t i=0; i<midiEventCount; ++i) {
      // only process regular midi even
      if (midiEvents[i].size > 1 and midiEvents[i].size <= 4) {
        // channel and type of event on first value
	int chan = midiEvents[i].data[0] & 0x0F;
        int type = midiEvents[i].data[0] & 0xF0;
        switch(type) {
          
          // note on
        case 144:
          if (midiEvents[i].size > 2) {
            synthFM_Voice_noteOn(context_processor, midiEvents[i].data[1], midiEvents[i].data[2], chan);
          }

          break;
          // note off
        case 128:
          if (midiEvents[i].size > 1) {
            synthFM_Voice_noteOff(context_processor, midiEvents[i].data[1], chan);
          }
          break;

          // cc
        case 176:
          if (midiEvents[i].size > 3) {
            // cc number and then value
            int cc = midiEvents[i].data[1];
            int value = midiEvents[i].data[2];
            switch(cc) {
              // sustain
            case 64:
              if (value >= 64) {
                synthFM_Voice_synthSetSustain(context_processor, true);
              } else {
                synthFM_Voice_synthSetSustain(context_processor, false);
              }
              break;
            }
          }
          break;

          // pitch bend
          // data: pitchbend value = ev.buffer[1] | (ev.buffer[2] << 7);
        case 224:
          if (midiEvents[i].size > 2) {
            // retrieve full value
            int pitchBend = midiEvents[i].data[1] | (midiEvents[i].data[2] << 7);
            float semitones = 0.0;
            // compute semitones, for now will bend +/- one tone
            // from 0 (-2 semitones) to 16383 (+2 semitones), 8192: no bend
            if (pitchBend > 8192) {
              semitones =  2.0 * (pitchBend - 8192) / (8191);
            }
            else if (pitchBend < 8192) {
              semitones =  - 2.0 * (8192 - pitchBend) / (8192);
            }
            synthFM_Voice_synthPitchBend(context_processor, float_to_fix(semitones));
          }
          break;
        }
      }
    }

    // deal with audio
    float *const out = outputs[0];

    // we will process in chunks
    uint32_t k = 0;
    while (k < frames) {
      // enough frames left for whole buffer or only leftovers?
      int chunkSize = ((frames - k) > BUFFER_SIZE )?BUFFER_SIZE:(frames - k);
      // process
      synthFM_Voice_process_bufferTo(context_processor, BUFFER_SIZE, buffOut);
      // copy to output buffer
      for (int i = 0; i < chunkSize; i++) {
        out[i] = fix_to_float(buffOut[k+i]);
      }
      // advance
      k += chunkSize;
    }
  }

  
private:
  synthFM_Voice_process_type context_processor;
  fix16_t buffOut[BUFFER_SIZE];

  float modulatorAttack;
  float modulatorDecay;
  float modulatorSustain;
  float modulatorRelease;
  float modulatorRatio;
  float modulatorWavetable;
  float modulatorWavetablePhase;
  float modulatorLevel;
  float modulatorMode;
  float modulatorShift;
  float modulatorFeedback;

  float carrierAttack;
  float carrierDecay;
  float carrierSustain;
  float carrierRelease;
  float carrierRatio;
  float carrierWavetable;
  float carrierWavetablePhase;


  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthFM);
};

Plugin *createPlugin() { return new SynthFM(); }

END_NAMESPACE_DISTRHO
