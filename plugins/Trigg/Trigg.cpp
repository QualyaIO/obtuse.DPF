
#include "ExtendedPlugin.hpp"
#include "utils.h"
#include "DistrhoPluginUtils.hpp"
#include <ctime>

START_NAMESPACE_DISTRHO

// in seconds, how long for each trigger -- to sync with Clock
#define TRIGGER_LENGTH 0.0002

// Wrapper for Trigg.
// Note: in this version there is a flag to reset on transport reset (effective upon next reset).
// TODO: alternate MIDI input/output to be used with Arp or Chord? 
class Trigg : public ExtendedPlugin {
public:
  // Note: do not care with default values since we will sent all parameters upon init
  Trigg() : ExtendedPlugin(kParameterCount, 0, 0) {
    utils_Trigg_process_init(context_processor);
    // seed with current second and cpu clock to get something more granular
    utils_Trigg_setSeed(context_processor, time(NULL) + (unsigned) clock());
  }

protected:
  // metadata
  const char *getLabel() const override { return "ObtuseTrigg"; }
  const char *getDescription() const override {
    return "I trigger things.";
  }
  const char *getMaker() const override { return "Qualya"; }
  uint32_t getVersion() const override { return d_version(0,1,0); }
  int64_t getUniqueId() const override { 
    return d_cconst('O','T','R','G');
  }

  // ports
  void initAudioPort(bool input, uint32_t index, AudioPort& port) override
  {
    if (input)
      {
        switch (index)
          {
          case 0:
            // trigger can happen through MIDI, hence optional
            port.hints   = kAudioPortIsCV;
            port.name    = "Trigger Input";
            port.symbol  = "trigg_in";
            return;
          }
      }
    else
      {
        switch (index)
          {
          case 0:
            // only way to trigger, hence not optional
            port.hints   = kAudioPortIsCV;
            port.name    = "Trigger Output";
            port.symbol  = "trigg_out";
            return;
          }
      }
    // It shouldn't reach here, but just in case if index is greater than 0.
    ExtendedPlugin::initAudioPort(input, index, port);
  }
  
  // params
  void initParameter (uint32_t index, Parameter& parameter) override {

    switch (index) {
    case kClockDivider:
      parameter.hints = kParameterIsInteger | kParameterIsAutomatable;
      parameter.name = "Clock divider";
      parameter.shortName = "clk div";
      parameter.symbol = "clockdivider";
      parameter.unit = "divider";
      // arbitrary max divider
      parameter.ranges.def = 2.0f;
      parameter.ranges.min = 1.0f;
      parameter.ranges.max = 1024.0f;
      break;
    case kClockShift:
      parameter.hints = kParameterIsInteger | kParameterIsAutomatable;
      parameter.name = "Clock shift";
      parameter.shortName = "clk sht";
      parameter.symbol = "clockshift";
      parameter.unit = "shift";
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1023.0f;
      break;
    case kLoopLength:
      parameter.hints = kParameterIsInteger | kParameterIsAutomatable;
      parameter.name = "Loop length";
      parameter.shortName = "loop lgth";
      parameter.symbol = "looplength";
      parameter.unit = "steps";
      parameter.ranges.def = 16.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 128.0f;
      break;
    case kStartPos:
      parameter.hints = kParameterIsInteger | kParameterIsAutomatable;
      parameter.name = "Loop start position";
      parameter.shortName = "loop start";
      parameter.symbol = "loopstart";
      parameter.unit = "step";
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 127.0f;
      break;
    case kDensity:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Density";
      parameter.shortName = "dsty";
      parameter.symbol = "density";
      parameter.unit = "ratio";
      parameter.ranges.def = 0.5f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;
    case kBalance:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Balance";
      parameter.shortName = "blce";
      parameter.symbol = "balance";
      parameter.unit = "ratio";
      parameter.ranges.def = 0.5f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;
    case kEvolve:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Evolve";
      parameter.shortName = "evlv";
      parameter.symbol = "evolve";
      parameter.unit = "ratio";
      parameter.ranges.def = 0.5f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;
    case kMagnitude:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Magnitude";
      parameter.shortName = "mag";
      parameter.symbol = "magnitude";
      parameter.unit = "ratio";
      parameter.ranges.def = 0.5f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;
    case kDoTransportReset:
      parameter.hints = kParameterIsAutomatable | kParameterIsBoolean;
      parameter.name = "Reset on transport reset";
      parameter.shortName = "trans rst";
      parameter.symbol = "resetontransport";
      parameter.unit = "toggle";
      parameter.ranges.def = 1.0f;
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

    case kClockDivider:
      return clockDivider;
    case kClockShift:
      return clockShift;
    case kLoopLength:
      return loopLength;
    case kStartPos:
      return startPos;
    case kDensity:
      return density;
    case kBalance:
      return balance;
    case kEvolve:
      return evolve;
    case kMagnitude:
      return magnitude;
    case kDoTransportReset:
      return doTransportReset;

    default:
      return 0.0;
    }
  }
  
  void setParameterValue(uint32_t index, float value) override {
    // FIXME: check up to which point function is repeatedly called from host even when value does not change
    switch (index) {
    case kClockDivider:
      clockDivider = value;
      utils_Trigg_setDivider(context_processor, clockDivider);
      break;
    case kClockShift:
      clockShift = value;
      utils_Trigg_setShift(context_processor, clockShift);
      break;
    case kLoopLength:
      loopLength = value;
      utils_Trigg_setLength(context_processor, loopLength);
      break;
    case kStartPos:
      startPos = value;
      utils_Trigg_setPosition(context_processor, startPos);
      break;
    case kDensity:
      density = value;
      utils_Trigg_setDensity(context_processor, float_to_fix(density));
      break;
    case kBalance:
      balance = value;
      utils_Trigg_setBalance(context_processor, float_to_fix(balance));
      break;
    case kEvolve:
      evolve = value;
      utils_Trigg_setEvolve(context_processor, float_to_fix(evolve));
      break;
    case kMagnitude:
      magnitude = value;
      utils_Trigg_setMagnitude(context_processor, float_to_fix(magnitude));
      break;
    case kDoTransportReset:
      doTransportReset = value;

    default:
      break;
    }
  }


  void process(uint32_t chunkSize, uint32_t) {
    // checking reset
    const TimePosition& timePos(getTimePosition());
    if (timePos.playing) {
      if (!transportReset and timePos.frame == 0) {
        transportReset = true;
        // we constantly check transport reset so we are in sync once option is set
        if (doTransportReset) {
          utils_Trigg_reset(context_processor);
        }
      }
      else if (timePos.frame > 0) {
        transportReset = false;
      }
    }

    // take care of input, threshold 0.1 for trigger
    for (unsigned int i = 0; i < chunkSize; i++) {
      if (fix_to_float(buffIn[i]) >= 0.1 and !triggerringIn) {
        triggerringIn = true;
        int trig = utils_Trigg_process(context_processor);
        // new output trigger
        if (trig) {
          triggerringOut = true;
          tickOut = tick;
        }
      }
      // turn off note
      else if (fix_to_float(buffIn[i]) < 0.1 and triggerringIn) {
        triggerringIn = false;
      }

      // output
      if (triggerringOut and (tick - tickOut) / getSampleRate() >= TRIGGER_LENGTH) {
        triggerringOut = false;
      }
      buffOut[i] = triggerringOut ? float_to_fix(1.0) : float_to_fix(0.0);

      tick++;
    }
  }

private:
  utils_Trigg_process_type context_processor;

  // total frame count
  unsigned long int tick = 0;
  // input and output are triggers
  bool triggerringIn = false;
  bool triggerringOut = false;
  // frame count for last trigger
  unsigned long int tickOut = 0;
  // did we reset clock when back to 0 in transport ?
  bool transportReset = false;

  // parameters
  int clockDivider;
  int clockShift;
  int loopLength;
  int startPos;
  float density;
  float balance;
  float evolve;
  float magnitude;
  int doTransportReset;

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Trigg);
};

Plugin *createPlugin() { return new Trigg(); }

END_NAMESPACE_DISTRHO
