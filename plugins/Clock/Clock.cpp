
#include "ExtendedPlugin.hpp"
#include "utils.h"
#include "DistrhoPluginUtils.hpp"


START_NAMESPACE_DISTRHO

// in seconds, how long for each trigger
#define TRIGGER_LENGTH 0.0002

// more explicit outputs
enum ClockOutputs {
                   OUT_BEAT, // beat also in transport
                   OUT_FIRST_BEAT, // bar in transport
                   OUT_FIRST_GROUP, // note in transport (see kNoteBarRatio)
                   OUT_SECOND_GROUP, // same as OUT_FIRST_GROUP in transport
                   OUT_TICKS, // tick also in transport
                   NB_OUTS
};

// Wrapper for Clock.
// outputs 0.2ms triggers
// TODO: MIDI input to sync directly with it without relying to host?
// NOTE: in autotomous follow transport it will only pause at the same time of transport (albeit at risk of loosing sync upon pause/play with current implementation) and reset when we are back to 0 (if supported by plugin type and host). Jumping forward or backward will otherwise let the clock run.
// NOTE: in transport, will retrigger upon change in timeline
// NOTE: in transport only note to beat ratio parameter is used, bypass obtuse DSP.
// NOTE: host time pos not supported in LADSPA/DSSI versions
// NOTE: when host handles poorly multiple outputs, user can override with desired information -- e.g. the first two ones
class Clock : public ExtendedPlugin {
public:
  // Note: do not care with default values since we will sent all parameters upon init
  Clock() : ExtendedPlugin(kParameterCount, 0, 0) {
    utils_Clock_process_init(context_processor);
    // initialize info about outputs
    for (int i = 0; i < NB_OUTS; i++) {
      triggerringOut[i] = false;
      tickOut[i] = 0;
      // will also be set in initParameter
      outputSelect[i] = i;
    }
  }

protected:
  // metadata
  const char *getLabel() const override { return "ObtuseClock"; }
  const char *getDescription() const override {
    return "I clock things.";
  }
  const char *getMaker() const override { return "Qualya"; }
  uint32_t getVersion() const override { return d_version(0,1,0); }
  int64_t getUniqueId() const override { 
    return d_cconst('O','C','L','K');
  }

  // ports
  // TODO: leave names even if can be overrode?
  void initAudioPort(bool input, uint32_t index, AudioPort& port) override
  {
    if (!input)
      {
        // NOTE: to sync with initParameter and default override
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
      parameter.hints = kParameterIsInteger | kParameterIsAutomatable;
      parameter.name = "Source";
      parameter.shortName = "source";
      parameter.symbol = "source";
      parameter.unit = "source";
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
      parameter.unit = "BPM";
      parameter.ranges.def = 120.0f;
      parameter.ranges.min = fix_to_float(utils_Clock_getMinBPM());
      parameter.ranges.max = fix_to_float(utils_Clock_getMaxBPM());
      break;
    case kTicks:
      parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
      parameter.name = "Ticks";
      parameter.shortName = "Ticks";
      parameter.symbol = "ticks";
      parameter.unit = "PPQN";
      parameter.ranges.def = 24.0f;
      parameter.ranges.min = utils_Clock_getMinTicks();
      parameter.ranges.max = utils_Clock_getMaxTicks();
      break;
    case kSwing:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Swing";
      parameter.shortName = "swing";
      parameter.symbol = "swing";
      parameter.unit = "ratio";
      parameter.ranges.def = 0.5f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;
    case kGroupSize:
      parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
      parameter.name = "Group size";
      parameter.shortName = "grp size";
      parameter.symbol = "groupsize";
      parameter.unit = "beats";
      parameter.ranges.def = 4.0f;
      parameter.ranges.min = utils_Clock_getMinGroupSize();
      parameter.ranges.max = utils_Clock_getMaxGroupSize();
      break;
    case kGroupRatio:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Group ratio";
      parameter.shortName = "grp ratio";
      parameter.symbol = "groupratio";
      parameter.unit = "ratio";
      parameter.ranges.def = 0.5f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;
    case kOrderMix:
      parameter.hints = kParameterIsAutomatable|kParameterIsBoolean;
      parameter.name = "Mix group order";
      parameter.shortName = "grp order";
      parameter.symbol = "grouporder";
      parameter.unit = "order";
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;
    case kNoteBarRatio:
      // used to select the duration of a "note" by taking the bar (mesure) as reference. Sligtly cumbersome way to enable any combination without knowledge of time signature.
      // with a 4/4 time signature, 0.25 will be a quarter note, 1 a full note, 0.3333 a third of a note. 
      // NOTE: ratio which are not 1/integer will create intervals not equidistant (not a bug, a feature -- otherwise use replace with an integer param to be used as the divider)
      // TODO: use instead (or also) time signature and pre-defined choice of notes?
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Note to bar ratio";
      parameter.shortName = "noteBar";
      parameter.symbol = "notetobar";
      parameter.unit = "ratio";
      // arbitrary range, but limit to 1 max to avoid sync issue when moving around within transport
      parameter.ranges.def = 0.25f;
      parameter.ranges.min = 1.0f/256;
      parameter.ranges.max = 1.0f;
      break;
    case kOverrideOut1:
      parameter.hints = kParameterIsInteger | kParameterIsAutomatable;
      parameter.name = "Override output 1";
      parameter.shortName = "out1";
      parameter.symbol = "overrideout1";
      parameter.unit = "type";
      parameter.enumValues.count = NB_OUTS;
      parameter.enumValues.restrictedMode = true;
      {
        ParameterEnumerationValue* const values = new ParameterEnumerationValue[parameter.enumValues.count];
        parameter.enumValues.values = values;
        values[0].label = "Default (beat)";
        values[0].value = OUT_BEAT;
        values[1].label = "First beat or bar";
        values[1].value = OUT_FIRST_BEAT;
        values[2].label = "First group or note";
        values[2].value = OUT_FIRST_GROUP;
        values[3].label = "Second group or note";
        values[3].value = OUT_SECOND_GROUP;
        values[4].label = "Ticks";
        values[4].value = OUT_TICKS;
      }
      // select default idx
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = NB_OUTS - 1;
      break;
    case kOverrideOut2:
      parameter.hints = kParameterIsInteger | kParameterIsAutomatable;
      parameter.name = "Override output 2";
      parameter.shortName = "out2";
      parameter.symbol = "overrideout2";
      parameter.unit = "type";
      parameter.enumValues.count = NB_OUTS;
      parameter.enumValues.restrictedMode = true;
      {
        ParameterEnumerationValue* const values = new ParameterEnumerationValue[parameter.enumValues.count];
        parameter.enumValues.values = values;
        values[0].label = "Beat";
        values[0].value = OUT_BEAT;
        values[1].label = "Default (first beat or bar)";
        values[1].value = OUT_FIRST_BEAT;
        values[2].label = "First group or note";
        values[2].value = OUT_FIRST_GROUP;
        values[3].label = "Second group or note";
        values[3].value = OUT_SECOND_GROUP;
        values[4].label = "Ticks";
        values[4].value = OUT_TICKS;
      }
      // select default idx
      parameter.ranges.def = 1.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = NB_OUTS - 1;
      break;
    case kOverrideOut3:
      parameter.hints = kParameterIsInteger | kParameterIsAutomatable;
      parameter.name = "Override output 3";
      parameter.shortName = "out3";
      parameter.symbol = "overrideout3";
      parameter.unit = "type";
      parameter.enumValues.count = NB_OUTS;
      parameter.enumValues.restrictedMode = true;
      {
        ParameterEnumerationValue* const values = new ParameterEnumerationValue[parameter.enumValues.count];
        parameter.enumValues.values = values;
        values[0].label = "Beat";
        values[0].value = OUT_BEAT;
        values[1].label = "First beat or bar";
        values[1].value = OUT_FIRST_BEAT;
        values[2].label = "Default (first group or note)";
        values[2].value = OUT_FIRST_GROUP;
        values[3].label = "Second group or note";
        values[3].value = OUT_SECOND_GROUP;
        values[4].label = "Ticks";
        values[4].value = OUT_TICKS;
      }
      // select default idx
      parameter.ranges.def = 2.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = NB_OUTS - 1;
      break;
    case kOverrideOut4:
      parameter.hints = kParameterIsInteger | kParameterIsAutomatable;
      parameter.name = "Override output 4";
      parameter.shortName = "out4";
      parameter.symbol = "overrideout4";
      parameter.unit = "type";
      parameter.enumValues.count = NB_OUTS;
      parameter.enumValues.restrictedMode = true;
      {
        ParameterEnumerationValue* const values = new ParameterEnumerationValue[parameter.enumValues.count];
        parameter.enumValues.values = values;
        values[0].label = "Beat";
        values[0].value = OUT_BEAT;
        values[1].label = "First beat or bar";
        values[1].value = OUT_FIRST_BEAT;
        values[2].label = "First group or note";
        values[2].value = OUT_FIRST_GROUP;
        values[3].label = "Default (second group or note)";
        values[3].value = OUT_SECOND_GROUP;
        values[4].label = "Ticks";
        values[4].value = OUT_TICKS;
      }
      // select default idx
      parameter.ranges.def = 3.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = NB_OUTS - 1;
      break;
    case kOverrideOut5:
      parameter.hints = kParameterIsInteger | kParameterIsAutomatable;
      parameter.name = "Override output 5";
      parameter.shortName = "out5";
      parameter.symbol = "overrideout5";
      parameter.unit = "type";
      parameter.enumValues.count = NB_OUTS;
      parameter.enumValues.restrictedMode = true;
      {
        ParameterEnumerationValue* const values = new ParameterEnumerationValue[parameter.enumValues.count];
        parameter.enumValues.values = values;
        values[0].label = "Beat";
        values[0].value = OUT_BEAT;
        values[1].label = "First beat or bar";
        values[1].value = OUT_FIRST_BEAT;
        values[2].label = "First group or note";
        values[2].value = OUT_FIRST_GROUP;
        values[3].label = "Second group or note";
        values[3].value = OUT_SECOND_GROUP;
        values[4].label = "Default (ticks)";
        values[4].value = OUT_TICKS;
      }
      // select default idx
      parameter.ranges.def = 4.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = NB_OUTS - 1;
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
    case kOverrideOut1:
      return outputSelect[OUT_BEAT];
    case kOverrideOut2:
      return outputSelect[OUT_FIRST_BEAT];
    case kOverrideOut3:
      return outputSelect[OUT_FIRST_GROUP];
    case kOverrideOut4:
      return outputSelect[OUT_SECOND_GROUP];
    case kOverrideOut5:
      return outputSelect[OUT_TICKS];

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
    case kOverrideOut1:
      outputSelect[OUT_BEAT] = value;
      break;
    case kOverrideOut2:
      outputSelect[OUT_FIRST_BEAT] = value;
      break;
    case kOverrideOut3:
      outputSelect[OUT_FIRST_GROUP] = value;
      break;
    case kOverrideOut4:
      outputSelect[OUT_SECOND_GROUP] = value;
      break;
    case kOverrideOut5:
      outputSelect[OUT_TICKS] = value;
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

    // outputs for each sample
    float outs[NB_OUTS];

    const TimePosition& timePos(getTimePosition());

    // autonomous mode, rely on obtuse
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

        // we run all outputs no matter the overrides to keep them in sync upon change
        // setting output, any beat is a good beat
        outs[OUT_BEAT] = triggerVal(OUT_BEAT, ret > 0, tick);
        // only first beat of group
        outs[OUT_FIRST_BEAT] = triggerVal(OUT_FIRST_BEAT, ret == 1, tick);
        // beats on first group
        outs[OUT_FIRST_GROUP] = triggerVal(OUT_FIRST_GROUP, ret == 1 || ret == 2, tick);
        // beats on second group
        outs[OUT_SECOND_GROUP] = triggerVal(OUT_SECOND_GROUP, ret == 3, tick);
        // ticks from clock
        outs[OUT_TICKS] = triggerVal(OUT_TICKS, newTicks > 0, tick);
        
        // redirect outputs
        out_beat[i] = outs[outputSelect[OUT_BEAT]];
        out_first_beat[i] = outs[outputSelect[OUT_FIRST_BEAT]];
        out_first_group[i] = outs[outputSelect[OUT_FIRST_GROUP]];
        out_second_group[i] = outs[outputSelect[OUT_SECOND_GROUP]];
        out_ticks[i] = outs[outputSelect[OUT_TICKS]];
        
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
          outs[OUT_BEAT] = triggerVal(OUT_BEAT, detectBeat, tick);
          // here bar
          outs[OUT_FIRST_BEAT] = triggerVal(OUT_FIRST_BEAT, detectBar, tick);
          // note
          outs[OUT_FIRST_GROUP] = triggerVal(OUT_FIRST_GROUP, detectNote, tick);
          // same output, but duplicate to sync trigger
          outs[OUT_SECOND_GROUP] = triggerVal(OUT_SECOND_GROUP, detectNote, tick);
          // ticks
          outs[OUT_TICKS] = triggerVal(OUT_TICKS, detectTick, tick);
          // redirect outputs
          out_beat[i] = outs[outputSelect[OUT_BEAT]];
          out_first_beat[i] = outs[outputSelect[OUT_FIRST_BEAT]];
          out_first_group[i] = outs[outputSelect[OUT_FIRST_GROUP]];
          out_second_group[i] = outs[outputSelect[OUT_SECOND_GROUP]];
          out_ticks[i] = outs[outputSelect[OUT_TICKS]];
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
  // check last triggers for transport
  int transportLastTick = 0;
  int transportLastBeat = 0;
  int transportLastBar = 0;
  int transportLastNote = 0;
  // user can override outputs
  int outputSelect[NB_OUTS];

  // parameters
  int source;
  float BPM;
  float swing;
  int orderMix;
  int groupSize;
  float groupRatio;
  int ticks;
  float noteBarRatio;

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Clock);
};

Plugin *createPlugin() { return new Clock(); }

END_NAMESPACE_DISTRHO
