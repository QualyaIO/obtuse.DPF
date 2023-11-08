
#include "DistrhoPlugin.hpp"
#include "effects.h"

START_NAMESPACE_DISTRHO

// chunk size to process audio
// to sync with vult code
#define BUFFER_SIZE 128

class Saturator : public Plugin {
public:
  Saturator() : Plugin(kParameterCount, 0, 0), threshold(0.8), coeff(1.0) {
    effects_Saturator_process_init(context_processor);
  }

protected:
  // metadata
  const char *getLabel() const override { return "BotaniaSaturator"; }
  const char *getDescription() const override {
    return "I clip/saturate things.";
  }
  const char *getMaker() const override { return "jfrey"; }
  const char *getLicense() const override { return "Custom"; }
  uint32_t getVersion() const override { return d_version(1,0,0); }
  int64_t getUniqueId() const override { 
    return d_cconst('B','S','A','T'); 
  }

  // params
  void initParameter (uint32_t index, Parameter& parameter) override {
    // tODO: should we retrieve default from actual value (after init)?
    switch (index) {
    case kThreshold:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Threshold";
      parameter.symbol = "threshold";
      parameter.ranges.def = 0.8f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      effects_Saturator_setThreshold(context_processor, float_to_fix(parameter.ranges.def));
      break;
    case kCoeff:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Coeff";
      parameter.symbol = "coeff";
      parameter.ranges.def = 1.0f;
      parameter.ranges.min = fix_to_float(effects_Saturator_getMinCoeff());
      parameter.ranges.max = fix_to_float(effects_Saturator_getMaxCoeff());
      effects_Saturator_setCoeff(context_processor, float_to_fix(parameter.ranges.def));
      break;
    default:
      break;
    }
  }

  float getParameterValue(uint32_t index) const override {
    switch (index) {
    case kThreshold:
      return threshold;
    case kCoeff:
      return coeff;
    default:
      return 0.0;
    }
  }
  
  void setParameterValue(uint32_t index, float value) override {
    // FIXME: check up to which point function is repeatedly called from host even when value does not change
    switch (index) {
    case kThreshold:
      effects_Saturator_setThreshold(context_processor, float_to_fix(value));
      threshold = value;
      break;
    case kCoeff:
      effects_Saturator_setCoeff(context_processor, float_to_fix(value));
      coeff = value;
      break;
    default:
      break;
    }
  }
  
  void run(const float **inputs, float **outputs, uint32_t frames) override {
    const float *const in = inputs[0];
    float *const out = outputs[0];

    // we will process in chunks
    uint32_t k = 0;
    while (k < frames) {
      // enough frames left for whole buffer or only leftovers?
      int chunkSize = ((frames - k) > BUFFER_SIZE )?BUFFER_SIZE:(frames - k);
      // copy to input buffer
      for (int i = 0; i < chunkSize; i++) {
        buffIn[i] = float_to_fix(in[k+i]);
      }
      // process
      effects_Saturator_process_bufferTo(context_processor, chunkSize, buffIn, buffOut);
      // copy to output buffer
      for (int i = 0; i < chunkSize; i++) {
        out[i] = fix_to_float(buffOut[k+i]);
      }
      // advance
      k += chunkSize;
    }
  }

  
private:
  effects_Saturator_process_type context_processor;
  fix16_t buffIn[BUFFER_SIZE];
  fix16_t buffOut[BUFFER_SIZE];
  float threshold;
  float coeff;

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Saturator);
};

Plugin *createPlugin() { return new Saturator(); }

END_NAMESPACE_DISTRHO
