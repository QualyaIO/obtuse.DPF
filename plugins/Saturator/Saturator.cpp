
#include "DistrhoPlugin.hpp"

START_NAMESPACE_DISTRHO

class Saturator : public Plugin {
public:
  Saturator() : Plugin(kParameterCount, 0, 0), threshold(0.8), coeff(1.0) {}

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
    return d_cconst('B','O','S','A','T'); 
  }

  // params
  void initParameter (uint32_t index, Parameter& parameter) override {
    switch (index) {
    case kThreshold:
      parameter.name = "Threshold";
      parameter.symbol = "theshold";
      parameter.ranges.def = 1.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;
    case kCoeff:
      parameter.name = "Coeff";
      parameter.symbol = "coeff";
      // TODO: retrieve flom plugin
      parameter.ranges.def = 1.0f;
      parameter.ranges.min = -1000.0f;
      parameter.ranges.max = 1000.0f;
      break;
    default:
      break;
    }
  }

  float getParameterValue(uint32_t index) const override {
    switch (index) {
    case kThreshold:
      return theshold;
    case kCoeff:
      return gain;
    default:
      return 0.0;
    }
  }
  
  void setParameterValue(uint32_t index, float value) override {
    switch (index) {
    case kThreshold:
      threshold = value;
      break;
    case kCoeff:
      coeff = value;
      break;
    default:
      break;
    }
  }
  
  void run(const float **inputs, float **outputs, uint32_t frames) override {
    // TODO
    const float *const in = inputs[0];
    float *const out = outputs[0];
    
    for (uint32_t i = 0; i < frames; i++) {
      out[i] = in[i] * coeff;
    }
  }

  
private:
  float threshold;
  float coeff;

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Saturator);
}

  Plugin *createPlugin() { return new Saturator(); }

END_NAMESPACE_DISTRHO
