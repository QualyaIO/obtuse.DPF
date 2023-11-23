
#include "ExtendedPlugin.hpp"
#include "utils.h"

START_NAMESPACE_DISTRHO

// in seconds, how long for each trigger
#define TRIGGER_LENGTH 0.001

// more explicit outputs
enum ClockOutputs {
                   OUT_BEAT,
                   OUT_FIRST_BEAT,
                   OUT_FIRST_GROUP,
                   OUT_SECOND_GROUP,
                   OUT_TICKS,
                   NB_OUTS
};

// Wrapper for Clock.
// outputs 1ms triggers
class Clock : public ExtendedPlugin {
public:
  // Note: do not care with default values since we will sent all parameters upon init
  Clock() : ExtendedPlugin(kParameterCount, 0, 0) {
    utils_Clock_process_init(context_processor);
    // initialize info about outputs
    for (int i = 0; i < NB_OUTS; i++) {
      triggerringOut[i] = false;
      tickOut[i] = 0;
    }
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
          case OUT_BEAT:
            port.hints   = kAudioPortIsCV;
            port.name    = "Beat";
            port.symbol  = "beat";
            return;
          case OUT_FIRST_BEAT:
            port.hints   = kAudioPortIsCV;
            port.name    = "Fist beat";
            port.symbol  = "fist_beat";
            return;
          case OUT_FIRST_GROUP:
            port.hints   = kAudioPortIsCV;
            port.name    = "First group";
            port.symbol  = "first_group";
            return;
          case OUT_SECOND_GROUP:
            port.hints   = kAudioPortIsCV;
            port.name    = "Second group";
            port.symbol  = "second_group";
            return;
          case OUT_TICKS:
            port.hints   = kAudioPortIsCV;
            port.name    = "Tick";
            port.symbol  = "tick";
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

  // check state of said output and condition, returning value for trigger (or not), i.e. 1.0 or 0.0
  // outputNum: which output to consider. Warning: not checking bounds
  // flag: associated condition, will create gate of TRIGGER_LENGTH once switched false
  // tick: current tick, as in frame number
  // FIXME: using only current sampling rate for 1ms gate
  float triggerVal(int outputNum, bool flag, long unsigned int tick) {
    // starting trigger
    if (flag) {
      triggerringOut[outputNum] = true;
      tickOut[outputNum] = tick;
    }

    // check if trigger should turn off
    // FIXME: using only current sampling rate for 1ms gate
    if (triggerringOut[outputNum] and (tick - tickOut[outputNum]) / getSampleRate() >= TRIGGER_LENGTH) {
      triggerringOut[outputNum] = false;
    }

    return triggerringOut[outputNum] ? 1.0 : 0.0;
  }

  void process(uint32_t chunkSize, uint32_t) {
    double sampleRate = getSampleRate();
    for (uint32_t i = 0; i < chunkSize; i++) {
      // TODO: use more precise computation by keeping count of frame and changes in sample rate
      timeFract += 1./ sampleRate;
      while (timeFract >= 1.0) {
        timeFract -= 1;
        timeS +=  1;
      }
      utils_Clock_setTime(context_processor, timeS, float_to_fix(timeFract));
      int ret = utils_Clock_process(context_processor);

      // setting output, any beat is a good beat
      buffOut[i] = float_to_fix(triggerVal(OUT_BEAT, ret > 0, tick));

      tick++;
    }

    //float time = timeS + timeFract;

    // get time position
    //const TimePosition& timePos(getTimePosition());
    //d_stdout("frame: %d, time: %f, timePos.frame: %d, valid: %d, playing: %d, BPM: %f", frame, time, timePos.frame,  timePos.bbt.valid, timePos.playing, timePos.bbt.beatsPerMinute);
  }

private:
  utils_Clock_process_type context_processor;

  // for computing time based on frame count
  int timeS = 0;
  double timeFract = 0.0;
  // total frame count
  unsigned long int tick = 0;
  // outputs are triggers
  bool triggerringOut[NB_OUTS];
  // frame count for last trigger
  unsigned long int tickOut[NB_OUTS];


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
