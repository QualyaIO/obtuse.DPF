
#include "ExtendedPlugin.hpp"
#include "utils.h"
#include <cassert>

START_NAMESPACE_DISTRHO

// Wrapper for Clock.
class Clock : public ExtendedPlugin {
public:
  // Note: do not care with default values since we will sent all parameters upon init
  Clock() : ExtendedPlugin(kParameterCount, 0, 0) {
    utils_Clock_process_init(context_processor);
  }

protected:
  // metadata
  const char *getLabel() const override { return "BotaniaClock"; }
  const char *getDescription() const override {
    return "I clock things.";
  }
  const char *getMaker() const override { return "jfrey"; }
  const char *getLicense() const override { return "Custom"; }
  uint32_t getVersion() const override { return d_version(1,0,0); }
  int64_t getUniqueId() const override { 
    return d_cconst('B','C','L','K'); 
  }

  // ports
  void initAudioPort(bool input, uint32_t index, AudioPort& port) override
  {
    if (!input)
      {
        switch (index)
          {
          case 0:
            port.hints   = kAudioPortIsCV;
            port.name    = "Beat";
            port.symbol  = "beat";
            return;
          case 1:
            port.hints   = kAudioPortIsCV;
            port.name    = "Tick";
            port.symbol  = "tick";
            return;
          case 2:
            port.hints   = kAudioPortIsCV;
            port.name    = "Fist beat";
            port.symbol  = "fist_beat";
            return;
          case 3:
            port.hints   = kAudioPortIsCV;
            port.name    = "First group";
            port.symbol  = "first_group";
            return;
          case 4:
            port.hints   = kAudioPortIsCV;
            port.name    = "Second group";
            port.symbol  = "second_group";
            return;
          }
      }
    // It shouldn't reach here, but just in case if index is greater than 0.
    ExtendedPlugin::initAudioPort(input, index, port);
  }
  
  // params
  void initParameter (uint32_t index, Parameter& parameter) override {

    switch (index) {
    case kBPM:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "BPM";
      parameter.shortName = "BPM";
      parameter.symbol = "BPM";
      parameter.ranges.def = 120.0f;
      parameter.ranges.min = fix_to_float(utils_Clock_getMinBPM());
      parameter.ranges.max = fix_to_float(utils_Clock_getMaxBPM());
      break;
    case kTicks:
      parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
      parameter.name = "Ticks";
      parameter.shortName = "Ticks";
      parameter.symbol = "PPQN";
      parameter.ranges.def = 24.0f;
      parameter.ranges.min = utils_Clock_getMinTicks();
      parameter.ranges.max = utils_Clock_getMaxTicks();
      break;
    case kSwing:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Swing";
      parameter.shortName = "swing";
      parameter.symbol = "ratio";
      parameter.ranges.def = 0.5f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;
    case kGroupSize:
      parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
      parameter.name = "Group size";
      parameter.shortName = "grp size";
      parameter.symbol = "beats";
      parameter.ranges.def = 4.0f;
      parameter.ranges.min = utils_Clock_getMinGroupSize();
      parameter.ranges.max = utils_Clock_getMaxGroupSize();
      break;
    case kGroupRatio:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Group ratio";
      parameter.shortName = "grp ratio";
      parameter.symbol = "ratio";
      parameter.ranges.def = 0.5f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;
    case kOrderMix:
      parameter.hints = kParameterIsAutomatable|kParameterIsBoolean;
      parameter.name = "Mix group order";
      parameter.shortName = "grp order";
      parameter.symbol = "order";
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

    case kBPM:
      return BPM;
    case kTicks:
      return ticks;
    case kSwing:
      return swing;
    case kGroupSize:
      return groupSize;
    case kGroupRatio:
      return groupRatio;
    case kOrderMix:
      return orderMix;

    default:
      return 0.0;
    }
  }
  
  void setParameterValue(uint32_t index, float value) override {
    // FIXME: check up to which point function is repeatedly called from host even when value does not change
    switch (index) {
    case kBPM:
      BPM = value;
      utils_Clock_setBPM(context_processor, float_to_fix(BPM));
      break;
    case kTicks:
      ticks = value;
      utils_Clock_setNbTicks(context_processor, ticks);
      break;
    case kSwing:
      swing = value;
      utils_Clock_setSwing(context_processor, float_to_fix(swing));
      break;
    case kGroupSize:
      groupSize = value;
      utils_Clock_setGroupSize(context_processor, groupSize);
      break;
    case kGroupRatio:
      groupRatio = value;
      utils_Clock_setGroupRatio(context_processor, float_to_fix(groupRatio));
      break;
    case kOrderMix:
      orderMix = value;
      utils_Clock_setOrderMix(context_processor, orderMix);
      break;

    default:
      break;
    }
  }

  void process(uint32_t chunkSize, uint32_t frame) {
    // TODO
  }

private:
  utils_Clock_process_type context_processor;

  // parameters
  float BPM;
  float swing;
  int orderMix;
  int groupSize;
  float groupRatio;
  int ticks;

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Clock);
};

Plugin *createPlugin() { return new Clock(); }

END_NAMESPACE_DISTRHO
