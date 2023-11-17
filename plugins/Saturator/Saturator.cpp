
#include "ExtendedPlugin.hpp"
#include "effects.h"

START_NAMESPACE_DISTRHO

class Saturator : public ExtendedPlugin {
public:
  Saturator() : ExtendedPlugin(kParameterCount, 0, 0), threshold(0.8), coeff(1.0) {
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
    // TODO: should we retrieve default from actual value (after init)?
    switch (index) {
    case kThreshold:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Threshold";
      parameter.shortName = "Thresh";
      parameter.symbol = "threshold";
      parameter.ranges.def = 0.8f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      // effectively set parameter
      setParameterValue(index, parameter.ranges.def);
      break;
    case kCoeff:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Coeff";
      parameter.shortName = "Coeff";
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

  void process(uint32_t chunkSize, uint32_t) {
    effects_Saturator_process_bufferTo(context_processor, chunkSize, buffIn, buffOut);
  }
  
private:
  effects_Saturator_process_type context_processor;
  float threshold;
  float coeff;

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Saturator);
};

Plugin *createPlugin() { return new Saturator(); }

END_NAMESPACE_DISTRHO
