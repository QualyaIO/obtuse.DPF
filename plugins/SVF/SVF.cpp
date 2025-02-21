
#include "ExtendedPlugin.hpp"
#include "effects.h"

START_NAMESPACE_DISTRHO

// Wrapper for SVF
// NOTE: has a tendency to output values beyond -1..1 (e.g. with larg(ish) values of Q), better put a Saturator behind.
class SVF : public ExtendedPlugin {
public:
  SVF() : ExtendedPlugin(kParameterCount, 0, 0) {
    effects_SVF_process_init(context_processor);
    effects_SVF_setSamplerate(context_processor, float_to_fix((float)getSampleRate() / 1000.0f));
  }

protected:
  // metadata
  const char *getLabel() const override { return "ObtuseSVF"; }
  const char *getDescription() const override {
    return "I filter things.";
  }
  const char *getMaker() const override { return "jfrey"; }
  uint32_t getVersion() const override { return d_version(0,1,0); }
  int64_t getUniqueId() const override { 
    return d_cconst('O','S','V','F');
  }

  // params
  void initParameter (uint32_t index, Parameter& parameter) override {
    // TODO: should we retrieve default from actual value (after init)?
    switch (index) {
    case kType:
      parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
      parameter.name = "Filter type";
      parameter.shortName = "Type";
      parameter.symbol = "filtertype";
      parameter.unit = "filter";
      parameter.enumValues.count = 5;
      parameter.enumValues.restrictedMode = true;
      {
        ParameterEnumerationValue* const values = new ParameterEnumerationValue[parameter.enumValues.count];
        parameter.enumValues.values = values;
        values[0].label = "off";
        values[0].value = 0;
        values[1].label = "low pass";
        values[1].value = 1;
        values[2].label = "high pass";
        values[2].value = 2;
        values[3].label = "band pass";
        values[3].value = 3;
        values[4].label = "notch";
        values[4].value = 4;
      }
      // select default idx
      parameter.ranges.def = 1.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = (float) (parameter.enumValues.count - 1);
      break;
    case kFrequency:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Frequency";
      parameter.shortName = "Freq";
      parameter.symbol = "frequency";
      parameter.unit = "Hz";
      parameter.ranges.def = 5.0f;
      parameter.ranges.min = 0.0f;
      // FIXME: fixed max value here (same as in VCV), in the DSP code it is caped and related sampling rate and nyquist
      parameter.ranges.max = 22.0f;
      break;
    case kQ:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Q coeff";
      parameter.shortName = "Q";
      parameter.symbol = "qcoeff";
      parameter.unit = "coeff";
      // NOTE: arbitrary range
      parameter.ranges.def = 1.0f;
      parameter.ranges.min = 0.0001f;
      parameter.ranges.max = 100.0f;
      break;
    default:
      break;
    }

    // effectively set parameter
    setParameterValue(index, parameter.ranges.def);
  }

  float getParameterValue(uint32_t index) const override {
    switch (index) {
    case kType:
      return type;
    case kFrequency:
      return frequency;
    case kQ:
      return q;
    default:
      return 0.0;
    }
  }
  
  void setParameterValue(uint32_t index, float value) override {
    // FIXME: check up to which point function is repeatedly called from host even when value does not change
    switch (index) {
    case kType:
      type = value;
      effects_SVF_setType(context_processor, type);
      break;
    case kFrequency:
      frequency = value;
      effects_SVF_setFreq(context_processor, float_to_fix(frequency));
      break;
    case kQ:
      q = value;
      effects_SVF_setQ(context_processor, float_to_fix(q));
      break;
    default:
      break;
    }
  }

  void process(uint32_t chunkSize, uint32_t) {
    effects_SVF_process_bufferTo(context_processor, chunkSize, buffIn, buffOut);
  }

  // Optional callback to inform synth about a sample rate change on the plugin side.
  void sampleRateChanged(double newSampleRate) override
  {
    effects_SVF_setSamplerate(context_processor, float_to_fix((float)newSampleRate / 1000.0f));
  }
  
private:
  effects_SVF_process_type context_processor;

  // parameters
  int type ;
  float frequency;
  float q;

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SVF);
};

Plugin *createPlugin() { return new SVF(); }

END_NAMESPACE_DISTRHO
