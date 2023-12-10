
#include "ExtendedPlugin.hpp"
#include "MetaSampler.hpp"

START_NAMESPACE_DISTRHO

// Wrapper for all sampler synth
// FIXME: MPE flag is a hack, does not differentiate between channel, same pitch bend range applied to all
// TODO: available options such as voice re-use for same note
// NOTE: compared to VCV, the "override" switch when turned off will reload sampler to default 
// NOTE: while set to automatable, changing orreride or sample will reset playing notes, discard things like sustain or pitchbend
class Sampler : public ExtendedPlugin {
public:
  // Note: do not care with default values since we will sent all parameters upon init
  Sampler() : ExtendedPlugin(kParameterCount, 0, 0) {
  }

protected:
  // metadata
  const char *getLabel() const override { return "BotaniaSampler"; }
  const char *getDescription() const override {
    return "I do samples things.";
  }
  const char *getMaker() const override { return "jfrey"; }
  const char *getLicense() const override { return "Custom"; }
  uint32_t getVersion() const override { return d_version(1,0,0); }
  int64_t getUniqueId() const override { 
    return d_cconst('B','S','M','P'); 
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
    case kPitchBendRange:
      // TODO: use a dedicated param for master pitch bend and one for MPE pitch bend
      parameter.hints = kParameterIsInteger | kParameterIsAutomatable;
      parameter.name = "Pitch bend range";
      parameter.shortName = "PB range";
      parameter.symbol = "+/- semitones";
      // NOTE: default would be 48 for MPE messages (channel 2 to 16) and 2 only for master pitch bend (channel 1)
      parameter.ranges.def = 2.0f;
      parameter.ranges.min = 1.0f;
      parameter.ranges.max = 96.0f;
      break;
    case kSample:
      parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
      parameter.name = "Sample";
      parameter.shortName = "sample";
      parameter.symbol = "index";
      parameter.enumValues.count = MetaSampler::getNbSamples();
      parameter.enumValues.restrictedMode = true;
      {
        ParameterEnumerationValue* const values = new ParameterEnumerationValue[parameter.enumValues.count];
        parameter.enumValues.values = values;
        for (int i = 0; i < MetaSampler::getNbSamples(); i++) {
          values[i].label = String(MetaSampler::getSampleName(i));
          values[i].value = i;
        }
      }
      // select default idx
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = MetaSampler::getNbSamples()-1;
      break;
    case kOverride:
      parameter.hints = kParameterIsAutomatable|kParameterIsBoolean;
      parameter.name = "Override loop";
      parameter.shortName = "override";
      parameter.symbol = "override";
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;
    case kLoop:
      parameter.hints = kParameterIsAutomatable|kParameterIsBoolean;
      parameter.name = "Enable looping";
      parameter.shortName = "loop";
      parameter.symbol = "loop";
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;
    case kLoopStart:
      parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
      parameter.name = "Loop start";
      parameter.shortName = "loop S";
      parameter.symbol = "sample";
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = MetaSampler::getMaxLoopSize();
      break;
    case kLoopEnd:
      parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
      parameter.name = "Loop end";
      parameter.shortName = "loop E";
      parameter.symbol = "sample";
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = MetaSampler::getMaxLoopSize();
      break;
    case kEffectiveLoop:
      parameter.hints = kParameterIsAutomatable|kParameterIsBoolean|kParameterIsOutput;
      parameter.name = "Effective loop enabled";
      parameter.shortName = "dflt loop";
      parameter.symbol = "loop";
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;
    case kEffectiveLoopStart:
      parameter.hints = kParameterIsAutomatable|kParameterIsInteger|kParameterIsOutput;
      parameter.name = "Effective loop start";
      parameter.shortName = "dflt loop S";
      parameter.symbol = "sample";
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = MetaSampler::getMaxLoopSize();
      break;
    case kEffectiveLoopEnd:
      parameter.hints = kParameterIsAutomatable|kParameterIsInteger|kParameterIsOutput;
      parameter.name = "Effective loop end";
      parameter.shortName = "dflt loop E";
      parameter.symbol = "sample";
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = MetaSampler::getMaxLoopSize();
      break;
    case kSize:
      parameter.hints = kParameterIsAutomatable|kParameterIsInteger|kParameterIsOutput;
      parameter.name = "Sample size";
      parameter.shortName = "size";
      parameter.symbol = "samples";
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = MetaSampler::getMaxLoopSize();
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
    case kPitchBendRange:
      return pitchBendRange;
    case kSample:
      return sample;
    case kOverride:
      return overrideConfig;
    case kLoop:
      return loop;
    case kLoopStart:
      return loopStart;
    case kLoopEnd:
      return loopEnd;
    case kEffectiveLoop:
      return effectiveLoop;
    case kEffectiveLoopStart:
      return effectiveLoopStart;
    case kEffectiveLoopEnd:
      return effectiveLoopEnd;
    case kSize:
      return size;
      
    default:
      return 0.0;
    }
  }

  // change sampler if necessary, send all current parameter depending on override option -- also update output parameter
  void updateSampler() {
    // discard call if sample index is invalid (e.g. upon init override switch called before first sample is selected)
    if (sample >= 0) {
      sampler.switchTo(sample);
      // everything need to be handled at this level, meta sampler only takes car of init
      sampler.setSamplerate(getSampleRate()  / 1000.0f);
      if (overrideConfig) {
        sampler.setLoop(loop);
        sampler.setLoopStart(loopStart);
        sampler.setLoopEnd(loopEnd);
      }
      effectiveLoop = sampler.getLoop();
      effectiveLoopStart = sampler.getLoopStart();
      effectiveLoopEnd = sampler.getLoopEnd();
      size = sampler.getSize();
    }
  }
  
  void setParameterValue(uint32_t index, float value) override {
    // FIXME: check up to which point function is repeatedly called from host even when value does not change
    switch (index) {
    case kChannelInput:
      channelInput = value;
      break;
    case kPitchBendRange:
      pitchBendRange = value;
      break;
    case kSample:
      if (value != sample) {
        sample = value;
        updateSampler();
      }
      break;
    case kOverride:
      // apply parameter
      if (value != overrideConfig) {
        overrideConfig = value;
        updateSampler();
      }
      
      break;
    case kLoop:
      loop = value;
      if (overrideConfig) {
        sampler.setLoop(loop);
        effectiveLoop = sampler.getLoop();
      }
      break;
    case kLoopStart:
      loopStart = value;
      if (overrideConfig) {
        // enforcing range of loop within size
        // FIXME: getting too close to the end will disable crossfading and then loop altogether
        // TODO: also enforce start pos compared to end?
        int effectiveLoopStart = loopStart > sampler.getSize() ? sampler.getSize() : loopStart;

        sampler.setLoopStart(effectiveLoopStart);
        effectiveLoopStart = sampler.getLoopStart();
      }
      break;
    case kLoopEnd:
      loopEnd = value;
      if (overrideConfig) {
        // enforcing range of loop within size
        int effectiveLoopEnd = loopEnd > sampler.getSize() ? sampler.getSize() : loopEnd;
        sampler.setLoopEnd(effectiveLoopEnd);
        effectiveLoopEnd = sampler.getLoopEnd();
      }
      break;

    default:
      break;
    }
  }

  // callbacks for processing MIDI
  void noteOn(uint8_t note, uint8_t velocity, uint8_t channel, uint32_t) {
    // filter event depending on selected channel
    if (channelInput == 0 or channelInput - 1 == channel) {
      sampler.noteOn(note, velocity, channel);
    }
  }

  void noteOff(uint8_t note, uint8_t channel, uint32_t) {
    // filter event depending on selected channel
    if (channelInput == 0 or channelInput - 1 == channel) {
      sampler.noteOff(note, channel);
    }
  }

  void pitchbend(uint8_t channel, float ratio, uint32_t) {
    // filter event depending on selected channel
    if (channelInput == 0 or channelInput - 1 == channel) {
      sampler.setPitchBend(ratio * pitchBendRange);
    }
  }

  void sustain(uint8_t channel, bool flag, uint32_t) {
    // filter event depending on selected channel
    if (channelInput == 0 or channelInput - 1 == channel) {
      sampler.setSustain(flag);
    }
  }

  void process(uint32_t chunkSize, uint32_t) {
    sampler.process(chunkSize, buffOut);
  }

  // Optional callback to inform synth about a sample rate change on the plugin side.
  void sampleRateChanged(double newSampleRate) override
  {
    sampler.setSamplerate(newSampleRate / 1000.0f);
  }

private:
  MetaSampler sampler;

  // parameters
  int channelInput;
  int pitchBendRange;
  // init to impossible value to let first init happen
  int sample = -1;
  int overrideConfig;
  int loop;
  int loopStart;
  int loopEnd;
  int effectiveLoop = 0;
  int effectiveLoopStart = 0;
  int effectiveLoopEnd = 0;
  int size = 0;
  

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Sampler);
};

Plugin *createPlugin() { return new Sampler(); }

END_NAMESPACE_DISTRHO
