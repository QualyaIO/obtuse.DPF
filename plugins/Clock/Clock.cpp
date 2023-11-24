
#include "ExtendedPlugin.hpp"
#include "utils.h"

START_NAMESPACE_DISTRHO

// in seconds, how long for each trigger
#define TRIGGER_LENGTH 0.001

// more explicit outputs
enum ClockOutputs {
                   OUT_BEAT, // beat also in transport
                   OUT_FIRST_BEAT, // bar in transport
                   OUT_FIRST_GROUP, // note in transport (see kNoteBeatRatio)
                   OUT_SECOND_GROUP, // same as OUT_FIRST_GROUP in transport
                   OUT_TICKS, // tick also in transport
                   NB_OUTS
};

// Wrapper for Clock.
// outputs 1ms triggers
// TODO: MIDI input to sync directly with it without relying to host?
// NOTE: in autotomous follow transport it will only pause at the same time of transport (albeit at risk of loosing sync upon pause/play with current implementation) and reset when we are back to 0 (if supported by plugin type and host). Jumping forward or backward will otherwise let the clock run.
// NOTE: in transport only note to beat ratio parameter is used, bypass botania DSP.
// NOTE: host time pos not supported in LADSPA/DSSI versions
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
            port.name    = "First beat or bar";
            port.symbol  = "1stBeat_bar";
            return;
          case OUT_FIRST_GROUP:
            port.hints   = kAudioPortIsCV;
            port.name    = "First group or note";
            port.symbol  = "1stGroup_note";
            return;
          case OUT_SECOND_GROUP:
            port.hints   = kAudioPortIsCV;
            port.name    = "Second group or note";
            port.symbol  = "2ndGroup_note";
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
    case kSource:
      parameter.hints = kParameterIsInteger;
      parameter.name = "Source";
      parameter.shortName = "source";
      parameter.symbol = "source";
      parameter.enumValues.count = 3;
      parameter.enumValues.restrictedMode = true;
      {
        ParameterEnumerationValue* const values = new ParameterEnumerationValue[parameter.enumValues.count];
        parameter.enumValues.values = values;
        values[0].label = "Autonomous";
        values[0].value = 0;
        values[1].label = "Autonomous, follow transport";
        values[1].value = 1;
        values[2].label = "Transport";
        values[2].value = 2;
      }
      // select default idx
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 2.0f;
      break;
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
    case kNoteBeatRatio:
      // used to select the duration of a "note" by taking the beat as reference. Sligtly cumbersome way to enable any combination without knowledge of time signature.
      // with a x/4 time signature, 0.25 will be 16th note, 2.0 hale a note, 1.3333 a third of a note ((1/3) / (1/4)). 
      // TODO: use instead (or also) time signature and pre-defined choice of notes?
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Note to beat ratio";
      parameter.shortName = "noteBeat";
      parameter.symbol = "rato";
      // arbitrary range
      parameter.ranges.def = 0.25f;
      parameter.ranges.min = 1.0f/256;
      parameter.ranges.max = 2.0f;
      break;

    default:
      break;
    }
    
    // effectively set parameter
    setParameterValue(index, parameter.ranges.def);
  }

  float getParameterValue(uint32_t index) const override {
    switch (index) {

    case kSource:
      return source;
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
    case kNoteBeatRatio:
      return noteBeatRatio;

    default:
      return 0.0;
    }
  }
  
  void setParameterValue(uint32_t index, float value) override {
    // FIXME: check up to which point function is repeatedly called from host even when value does not change
    switch (index) {
    case kSource:
      source = value;
      break;
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
    case kNoteBeatRatio:
      noteBeatRatio = value;
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

  // we take care ourselves of the main loop (we don't have MIDI)
  void run(const float**, float** outputs, uint32_t frames) override {
    float *const out_beat = outputs[OUT_BEAT];
    float *const out_first_beat = outputs[OUT_FIRST_BEAT];
    float *const out_first_group = outputs[OUT_FIRST_GROUP];
    float *const out_second_group = outputs[OUT_SECOND_GROUP];
    float *const out_ticks = outputs[OUT_TICKS];

    const TimePosition& timePos(getTimePosition());

    // autonomous mode, rely on botania
    if (source == 0 or source == 1) {
      // detect reset
      // TODO: check that we go indeed back to 0 ticks at start of buffer for all host
      if (source == 1 and !transportReset and timePos.frame == 0) {
        utils_Clock_reset(context_processor);
        transportReset = true;
      }
      else if (timePos.frame > 0) {
        transportReset = false;
      }

      for (uint32_t i = 0; i < frames; i++) {
        // compute current clock, depending if we follow or not transport
        // TODO: use more precise computation by keeping count of frame and changes in sample rate
        // FIXME: loose sync upon start/stop in follow transport, probably because we cannot update this value per frame
        if (source == 0 or (source == 1 and timePos.playing)) {
          timeFract += 1./ getSampleRate();
          while (timeFract >= 1.0) {
            timeFract -= 1;
            timeS +=  1;
          }
          utils_Clock_setTime(context_processor, timeS, float_to_fix(timeFract));
        }
        
        // retrieve ticks first, only interested if we have at least one, we might just loose some trigger at some point if there is too much delay (e.g. buffer is too long and we skipped beats, check audioscene MIDI player implementation to circumvent that)
        // NOTE: gate 1ms will quickly be too long there is too many ticks or BPM
        int newTicks = utils_Clock_getNbNewTicks(context_processor);

        // retrive current beat
        int ret = utils_Clock_process(context_processor);
        
        // setting output, any beat is a good beat
        out_beat[i] = triggerVal(OUT_BEAT, ret > 0, tick);
        // only first beat of group
        out_first_beat[i] = triggerVal(OUT_FIRST_BEAT, ret == 1, tick);
        // beats on first group
        out_first_group[i] = triggerVal(OUT_FIRST_GROUP, ret == 1 || ret == 2, tick);
        // beats on second group
        out_second_group[i] = triggerVal(OUT_SECOND_GROUP, ret == 3, tick);
        // ticks from clock
        out_ticks[i] = triggerVal(OUT_TICKS, newTicks > 0, tick);
        
        tick++;
      }
    }
    // transport mode
    else {

      // only work if we can et info from host
      if (timePos.bbt.valid) {

        // compute step tick for each frame
        double secondsPerBeat = 60.0 / timePos.bbt.beatsPerMinute;
        double framesPerBeat  =  getSampleRate() * secondsPerBeat;
        double ticksPerFrame = 0.0;
        if (timePos.bbt.ticksPerBeat > 0.0) {
          ticksPerFrame = timePos.bbt.ticksPerBeat / framesPerBeat;
        }
        double framePerfectTick = timePos.bbt.tick;

        if (timePos.playing) {
          d_stdout("frames: %d, timePos.frame: %d, tick: %f, barStartTick: %f, BPM: %f", frames, timePos.frame, timePos.bbt.tick, timePos.bbt.barStartTick, timePos.bbt.beatsPerMinute);
          d_stdout("secondsPerBeat: %f, framesPerBeat: %f, ticksPerBeat: %f, ticksPerFrame: %f", secondsPerBeat, framesPerBeat, timePos.bbt.ticksPerBeat, ticksPerFrame);
        }

        for (uint32_t i = 0; i < frames; i++) {
          // accumulating tick for each frame
          if (timePos.playing) {
            framePerfectTick += ticksPerFrame; 
            // clamp to max
            if (framePerfectTick > timePos.bbt.ticksPerBeat) {
              framePerfectTick = timePos.bbt.ticksPerBeat;
            }
            d_stdout("i: %d, framePerfectTick: %f", i, framePerfectTick);
          }
          tick++;
        }
      }
      // no support for transport, nullify output
      else {
        for (uint32_t i = 0; i < frames; i++) {
          out_beat[i] = 0.0;
          out_first_beat[i] = 0.0;
          out_first_group[i] = 0.0;
          out_second_group[i] = 0.0;
          out_ticks[i] = 0.0;
        }
      }
    }
  }

private:
  utils_Clock_process_type context_processor;

  // for computing time based on frame count
  int timeS = 0;
  double timeFract = 0.0;
  // total frame count (surely poorly named)
  unsigned long int tick = 0;
  // outputs are triggers
  bool triggerringOut[NB_OUTS];
  // frame count for last trigger
  unsigned long int tickOut[NB_OUTS];
  // did we reset clock when back to 0 in transport ?
  bool transportReset = false;

  // parameters
  int source;
  float BPM;
  float swing;
  int orderMix;
  int groupSize;
  float groupRatio;
  int ticks;
  float noteBeatRatio;

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Clock);
};

Plugin *createPlugin() { return new Clock(); }

END_NAMESPACE_DISTRHO
