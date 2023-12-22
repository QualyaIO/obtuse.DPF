
#include "ExtendedPlugin.hpp"
#include "utils.h"
#include "DistrhoPluginUtils.hpp"


START_NAMESPACE_DISTRHO

// in seconds, how long for each trigger -- to sync with Clock
#define TRIGGER_LENGTH 0.0002

// Wrapper for Trigger.
// Note: in this version there is a flag to reset on transport reset, as with Clock
// TODO: alternate MIDI input/output to be used with Arp or Chord? 
class Trigger : public ExtendedPlugin {
public:
  // Note: do not care with default values since we will sent all parameters upon init
  Trigger() : ExtendedPlugin(kParameterCount, 0, 0) {
    utils_Trigger_process_init(context_processor);
  }

protected:
  // metadata
  const char *getLabel() const override { return "BotaniaTrigger"; }
  const char *getDescription() const override {
    return "I trigger things.";
  }
  const char *getMaker() const override { return "jfrey"; }
  const char *getLicense() const override { return "Custom"; }
  uint32_t getVersion() const override { return d_version(1,0,0); }
  int64_t getUniqueId() const override { 
    return d_cconst('B','T','R','G'); 
  }

  // ports
  void initAudioPort(bool input, uint32_t index, AudioPort& port) override
  {
    if (!input)
      {
        switch (index)
          {
          case 0:
            // only way to trigger, hence not optional
            port.hints   = kAudioPortIsCV;
            port.name    = "Trigger Input";
            port.symbol  = "trigg_in";
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
      parameter.symbol = "divider";
      // arbitrary max divider
      parameter.ranges.def = 2.0f;
      parameter.ranges.min = 1.0f;
      parameter.ranges.max = 1024.0f;
      break;
    case kClockShift:
      parameter.hints = kParameterIsInteger | kParameterIsAutomatable;
      parameter.name = "Clock shift";
      parameter.shortName = "clk sht";
      parameter.symbol = "shift";
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1023.0f;
      break;
    case kLoopLength:
      parameter.hints = kParameterIsInteger | kParameterIsAutomatable;
      parameter.name = "Loop length";
      parameter.shortName = "loop lgth";
      parameter.symbol = "steps";
      parameter.ranges.def = 16.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 128.0f;
      break;
    case kStartPos:
      parameter.hints = kParameterIsInteger | kParameterIsAutomatable;
      parameter.name = "Loop start position";
      parameter.shortName = "loop start";
      parameter.symbol = "step";
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 127.0f;
      break;
    case kDensity:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Density";
      parameter.shortName = "dsty";
      parameter.symbol = "ratio";
      parameter.ranges.def = 0.5f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;
    case kBalance:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Balance";
      parameter.shortName = "blce";
      parameter.symbol = "ratio";
      parameter.ranges.def = 0.5f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;
    case kEvolve:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Evolve";
      parameter.shortName = "evlv";
      parameter.symbol = "ratio";
      parameter.ranges.def = 0.5f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;
    case kMagnitude:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Magnitude";
      parameter.shortName = "mag";
      parameter.symbol = "ratio";
      parameter.ranges.def = 0.5f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;
    case kTransportReset:
      parameter.hints = kParameterIsAutomatable | kParameterIsBoolean;
      parameter.name = "Reset on transport reset";
      parameter.shortName = "trans rst";
      parameter.symbol = "rest";
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
    case kNoteBarRatio:
      return noteBarRatio;

    default:
      return 0.0;
    }
  }
  
  void setParameterValue(uint32_t index, float value) override {
    // FIXME: check up to which point function is repeatedly called from host even when value does not change
    switch (index) {
    case kSource:
      source = value;
      // will trigger upon next tick/beat/bar when switched to autonomous
      if (source == 2) {
        const TimePosition& timePos(getTimePosition());
        if (timePos.bbt.valid) {
          transportLastTick = timePos.bbt.tick;
          transportLastBeat = timePos.bbt.beat;
          transportLastBar = timePos.bbt.bar;
          transportLastNote = 0;
        }
      }
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
    case kNoteBarRatio:
      noteBarRatio = value;
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

    // HACK to safeguard against host not supporting CV
    if (out_beat == NULL or out_first_beat == NULL or out_first_group == NULL or out_second_group == NULL or out_ticks == NULL) {
      return;
    }

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

      // do we get something new here?
      bool detectTick = false;
      bool detectBeat = false;
      bool detectBar = false;
      bool detectNote = false;

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
        int frameBeat = timePos.bbt.beat;
        int frameBar = timePos.bbt.bar;

        // dealing with notes
        double notesPerBar = 1./noteBarRatio;
        // how many ticks in total in bar and in in current bar
        double ticksPerBar = timePos.bbt.beatsPerBar * timePos.bbt.ticksPerBeat;
        double ticksInBar = (frameBeat - 1) * timePos.bbt.ticksPerBeat + framePerfectTick;
        double framePerfectNote = 0.0;
        if (ticksInBar > 0.0) {
          framePerfectNote = (ticksInBar / ticksPerBar) * notesPerBar;
        }
        // step for note
        double notesPerFrame = 0.0;
        double framesPerBar = framesPerBeat * timePos.bbt.beatsPerBar;
        if (framesPerBar > 0) {
           notesPerFrame = notesPerBar / framesPerBar;
        }

        if (timePos.playing) {

          // detect reset
          // TODO: check that we go indeed back to 0 ticks at start of buffer for all host
          if (!transportReset and timePos.frame == 0) {
            detectBar = true;
            transportLastBar = frameBar;
            detectBeat = true;
            transportLastBeat = frameBeat;
            detectNote = true;
            transportLastNote = framePerfectNote;
            transportReset = true;

          }
          else if (timePos.frame > 0) {
            transportReset = false;
          }

          // detect change in position, or we missed bar/beat/tick
          // NOTE: side effect is that we will trigger upon repositioning
          // NOTE: detect separately each to enable "rescue" mode, it could be logical to retrig beat upon bar change if we reposition
          if (frameBar != transportLastBar) {
            detectBar = true;
            transportLastBar = frameBar;
          }
          if (frameBeat != transportLastBeat) {
            detectBeat = true;
            transportLastBeat = frameBeat;
          }
          if ((int) framePerfectTick != transportLastTick) {
            transportLastTick = framePerfectTick;
            if (transportLastTick > 0) {
              detectTick = true;
            }
          }
          if ((int) framePerfectNote != transportLastNote) {
            transportLastNote = framePerfectNote;
            detectNote = true;
          }

        }

        for (uint32_t i = 0; i < frames; i++) {
          if (timePos.playing) {
            // accumulating tick for each frame
            framePerfectTick += ticksPerFrame;  
            framePerfectNote+= notesPerFrame; 
            // detect new ticks
            if ((int) framePerfectTick > transportLastTick) {
              transportLastTick = framePerfectTick;
              detectTick = true;
            }
            // detect beats
            if (timePos.bbt.ticksPerBeat > 0) {
              while (framePerfectTick >= timePos.bbt.ticksPerBeat) {
                framePerfectTick -= timePos.bbt.ticksPerBeat;
                frameBeat += 1;
                transportLastBeat = frameBeat;
                transportLastTick = framePerfectTick;
                detectBeat = true;
              }
            }
            // now bars
            while (frameBeat >= timePos.bbt.beatsPerBar + 1) {
              frameBeat = frameBeat - timePos.bbt.beatsPerBar;
              transportLastBeat = frameBeat;
              frameBar += 1;
              transportLastBar = frameBar;
              detectBar = true;
              // new bar, reset note, so we sync here
              detectNote = true;
              framePerfectNote = 0;
              transportLastNote = framePerfectNote;
            }
            // now notes
            if ((int) framePerfectNote > transportLastNote) {
              transportLastNote = framePerfectNote;
              detectNote = true;
            }
            if (notesPerBar > 0) {
              while (framePerfectNote >= notesPerBar) {
                framePerfectNote -= notesPerBar;
                transportLastNote = framePerfectNote;
              }
            }
          }
          // beat as well
          out_beat[i] = triggerVal(OUT_BEAT, detectBeat, tick);
          // here bar
          out_first_beat[i] = triggerVal(OUT_FIRST_BEAT, detectBar, tick);
          // note
          out_first_group[i] = triggerVal(OUT_FIRST_GROUP, detectNote, tick);
          // same output, but duplicate to sync trigger
          out_second_group[i] = triggerVal(OUT_SECOND_GROUP, detectNote, tick);
          // ticks
          out_ticks[i] = triggerVal(OUT_TICKS, detectTick, tick);
          // reset flags and advance for next
          detectBeat = false;
          detectBar = false;
          detectTick = false;
          detectNote = false;
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

  // total frame count (surely poorly named)
  unsigned long int tick = 0;
  // outputs are triggers
  bool triggerringOut[NB_OUTS];
  // frame count for last trigger
  unsigned long int tickOut[NB_OUTS];
  // did we reset clock when back to 0 in transport ?
  bool transportReset = false;
  // check last triggers for transport
  int transportLastTick = 0;
  int transportLastBeat = 0;
  int transportLastBar = 0;
  int transportLastNote = 0;

  // parameters
  int source;
  float BPM;
  float swing;
  int orderMix;
  int groupSize;
  float groupRatio;
  int ticks;
  float noteBarRatio;

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Trigger);
};

Plugin *createPlugin() { return new Trigger(); }

END_NAMESPACE_DISTRHO
