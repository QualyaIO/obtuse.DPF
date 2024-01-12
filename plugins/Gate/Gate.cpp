
#include "ExtendedPlugin.hpp"
#include "utils.h"
#include <cassert>

START_NAMESPACE_DISTRHO

// Wrapper for Gate.
// NOTE: in this version there is an option to set duration as a ratio of the bar length, updated at each frame
// TODO: add duration as per bar or note with transport
// TODO: add option for immediate note off with transport
// FIXME: immediately change MIDI output channel, not taking care of matching noteOff with noteOff
// TODO: preserve channel between input and output? (e.g. use 16 Gate)
class Gate : public ExtendedPlugin {
public:
  // Note: do not care with default values since we will sent all parameters upon init
  Gate() : ExtendedPlugin(kParameterCount, 0, 0) {
    utils_Gate_process_init(context_processor);
  }

protected:
  // metadata
  const char *getLabel() const override { return "BotaniaGate"; }
  const char *getDescription() const override {
    return "I gate things.";
  }
  const char *getMaker() const override { return "jfrey"; }
  const char *getLicense() const override { return "Custom"; }
  uint32_t getVersion() const override { return d_version(1,0,0); }
  int64_t getUniqueId() const override { 
    return d_cconst('B','G','A','T'); 
  }
  
  // params
  void initParameter (uint32_t index, Parameter& parameter) override {

    switch (index) {
    case kChannelInput:
      parameter.hints = kParameterIsInteger | kParameterIsAutomatable;
      parameter.name = "Input MIDI channel";
      parameter.shortName = "in chan";
      parameter.symbol = "channel";
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
    case kChannelOutput:
      parameter.hints = kParameterIsInteger | kParameterIsAutomatable;
      parameter.name = "Output MIDI channel";
      parameter.shortName = "out chan";
      parameter.symbol = "channel";
      // 1-index here
      parameter.ranges.def = 1.0;
      parameter.ranges.min = 1.0f;
      parameter.ranges.max = 16.0f;
      break;
    case kDuration:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Gate duration";
      parameter.shortName = "gate dur";
      parameter.symbol = "seconds_or_ratio";
      // range min is 1ms from (current) DSP, max is arbitrary
      parameter.ranges.def = 0.2f;
      parameter.ranges.min = 0.001f;
      parameter.ranges.max = 10.0f;
      break;
    case kDurationModeBar:
      parameter.hints = kParameterIsAutomatable|kParameterIsBoolean;
      parameter.name = "Duration as bar ratio";
      parameter.shortName = "dur bar ratio";
      parameter.symbol = "toggle";
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
    case kChannelOutput:
      return channelOutput;
    case kDuration:
      return duration;
    case kDurationModeBar:
      return durationModeBar;

    default:
      return 0.0;
    }
  }

  // change gate duration depending on option (seconds or bar)
  void updateDuration() {
    float newDuration = duration;
    // convert from bar length to duration in seconds
    if (durationModeBar) {
      double secondsPerBar = 1.0;
      // update duration as per bar if possible
      const TimePosition& timePos(getTimePosition());
      if (timePos.bbt.valid) {
        secondsPerBar = 60.0 / timePos.bbt.beatsPerMinute * timePos.bbt.beatsPerBar;
      }
      newDuration = secondsPerBar * duration;
    }
    
    if (newDuration != lastDuration) {
      lastDuration = newDuration;
      utils_Gate_setDuration(context_processor, float_to_fix(lastDuration));
    }
  }
  
  void setParameterValue(uint32_t index, float value) override {
    // FIXME: check up to which point function is repeatedly called from host even when value does not change
    switch (index) {
    case kChannelInput:
      channelInput = value;
      break;
    case kChannelOutput:
      channelOutput = value;
      break;
    case kDuration:
      duration = value;
      updateDuration();
      break;
    case kDurationModeBar:
      durationModeBar = value;
      updateDuration();
    default:
      break;
    }
  }

  // callbacks for processing MIDI
  // keep track of xx last active notes, duplicate no matter the channel will reset position. Using gate's list
  void noteOn(uint8_t note, uint8_t velocity, uint8_t channel, uint32_t) {
    // filter event depending on selected channel
    if (channelInput == 0 or channelInput - 1 == channel) {
      // add note to list
      // FIXME: not updating clock here
      utils_Gate_noteOn(context_processor, note, velocity, channel);
    }
  }

  void process(uint32_t chunkSize, uint32_t frame) {
    // update duration (if necessary) depending on options
    updateDuration();
    // make it run
    for (unsigned int i = 0; i < chunkSize; i++) {
      // update clock
      timeFract += 1./ getSampleRate();
      while (timeFract >= 1.0) {
        timeFract -= 1;
        timeS +=  1;
      }
      utils_Gate_setTime(context_processor, timeS, float_to_fix(timeFract));

      // run gate
      utils_Gate_process(context_processor);
      int note = utils_Gate_getNoteOff(context_processor);
      while (note >= 0) {
        sendNoteOff(note, channelOutput, frame+i);
        note = utils_Gate_getNoteOff(context_processor);
      }
      note = utils_Gate_getNoteOn(context_processor);
      while (note >= 0) {
        sendNoteOn(note, 127, channelOutput, frame+i);
        note = utils_Gate_getNoteOn(context_processor);
      }
    }
  }

private:
  utils_Gate_process_type context_processor;

  // for computing time based on frame count
  int timeS = 0;
  double timeFract = 0.0;
  // last bar value, to avoid too many updated
  double lastDuration = -1;

  // parameters
  int channelInput;
  int channelOutput;
  float duration;
  int durationModeBar = false;

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Gate);
};

Plugin *createPlugin() { return new Gate(); }

END_NAMESPACE_DISTRHO
