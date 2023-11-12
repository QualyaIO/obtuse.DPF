
#include "ExtendedPlugin.hpp"
#include "utils.h"
#include <cassert>

START_NAMESPACE_DISTRHO

// Wrapper for Arp
class Arp : public ExtendedPlugin {
public:
  // Note: do not care with default values since we will sent all parameters upon init
  Arp() : ExtendedPlugin(kParameterCount, 0, 0) {
    utils_Arp_process_init(context_processor);
  }

protected:
  // metadata
  const char *getLabel() const override { return "BotaniaArp"; }
  const char *getDescription() const override {
    return "I arp things.";
  }
  const char *getMaker() const override { return "jfrey"; }
  const char *getLicense() const override { return "Custom"; }
  uint32_t getVersion() const override { return d_version(1,0,0); }
  int64_t getUniqueId() const override { 
    return d_cconst('B','A','R','P'); 
  }

  // params
  void initParameter (uint32_t index, Parameter& parameter) override {

    switch (index) {
    case kMode:
      parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
      parameter.name = "Mode";
      parameter.shortName = "Mode";
      parameter.symbol = "mode";
      // we want to make sure we are on par with API
      parameter.enumValues.count = 6;
      assert(
             (parameter.enumValues.count  == Processor_arp_getNbModes(context_processor) - 1) &&
             "Number of modes for arp different than expected (6)"
             );
      parameter.enumValues.restrictedMode = true;
      {
        ParameterEnumerationValue* const values = new ParameterEnumerationValue[parameter.enumValues.count];
        parameter.enumValues.values = values;
        values[0].label = "up";
        values[0].value = 0;
        values[1].label = "down";
        values[1].value = 1;
        values[2].label = "up-down no repeat";
        values[2].value = 2;
        values[3].label = "up-down repea";
        values[3].value = 3;
        values[4].label = "down-up no repeat";
        values[4].value = 4;
        values[5].label = "down-up repeat";
        values[5].value = 5;
      }
      // select default idx
      parameter.ranges.def = 0.0f;
      break;
    case kRandNotes:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Random notes amount";
      parameter.shortName = "Rand notes";
      parameter.symbol = "amount";
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;
    case kRandomize:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Randomize probability";
      parameter.shortName = "Randomize";
      parameter.symbol = "p";
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

    case kMode:
      return mode;
    case kRandNotes:
      return randNotes;
    case kRandomize:
      return randomize;

    default:
      return 0.0;
    }
  }
  
  void setParameterValue(uint32_t index, float value) override {
    // FIXME: check up to which point function is repeatedly called from host even when value does not change
    switch (index) {
    case kMode:
      mode = value;
      utils_Arp_setMode(context_processor, value);
      break;
    case kRandNotes:
      randNotes = value;
      utils_Arp_setPRandomNotes(context_processor, float_to_fix(value));
      break;
    case kRandomize:
      randomize = value;
      utils_Arp_setPRandomize(context_processor, float_to_fix(value));
      break;

    default:
      break;
    }
  }

  // callbacks for processing MIDI
  void noteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
  }

  void noteOff(uint8_t note, uint8_t channel) {
  }

  void sustain(uint8_t, bool flag) {
  }

  void process(unsigned int chunkSize) {
  }

private:
  utils_Arp_process_type context_processor;

  float mode;
  float randNotes;
  float randomize;

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Arp);
};

Plugin *createPlugin() { return new Arp(); }

END_NAMESPACE_DISTRHO
