
#include "ExtendedPlugin.hpp"
#include "MetaDrummer.hpp"

START_NAMESPACE_DISTRHO

// Wrapper for all drummer  synth
// FIXME: MPE flag is a hack, does not differentiate between channel, same pitch bend range applied to all
// TODO: available options such as voice re-use for same note
class Drummer : public ExtendedPlugin {
public:
  // Note: do not care with default values since we will sent all parameters upon init
  Drummer() : ExtendedPlugin(kParameterCount, 0, 0) {
  }

protected:
  // metadata
  const char *getLabel() const override { return "ObtuseDrummer"; }
  const char *getDescription() const override {
    return "I doum doum things.";
  }
  const char *getMaker() const override { return "jfrey"; }
  uint32_t getVersion() const override { return d_version(0,1,0); }
  int64_t getUniqueId() const override { 
    return d_cconst('O','D','R','M');
  }

  // params
  void initParameter (uint32_t index, Parameter& parameter) override {

    switch (index) {
    case kChannelInput:
      parameter.hints = kParameterIsInteger | kParameterIsAutomatable;
      parameter.name = "Input MIDI channel";
      parameter.shortName = "in chan";
      parameter.symbol = "inchannel";
      parameter.unit = "channel";
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
      parameter.symbol = "pitchbend";
      parameter.unit = "semitones";
      // NOTE: default would be 48 for MPE messages (channel 2 to 16) and 2 only for master pitch bend (channel 1)
      parameter.ranges.def = 2.0f;
      parameter.ranges.min = 1.0f;
      parameter.ranges.max = 96.0f;
      break;
    case kKit:
      parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
      parameter.name = "Kit";
      parameter.shortName = "kit";
      parameter.symbol = "kit";
      parameter.unit = "index";
      parameter.enumValues.count = MetaDrummer::getNbKits();
      parameter.enumValues.restrictedMode = true;
      {
        ParameterEnumerationValue* const values = new ParameterEnumerationValue[parameter.enumValues.count];
        parameter.enumValues.values = values;
        for (int i = 0; i < MetaDrummer::getNbKits(); i++) {
          values[i].label = String(MetaDrummer::getKitName(i));
          values[i].value = i;
        }
      }
      // select default idx
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = MetaDrummer::getNbKits()-1;
      break;
    case kReuse:
      parameter.hints = kParameterIsAutomatable|kParameterIsBoolean;
      parameter.name = "Reuse voices";
      parameter.shortName = "reuse";
      parameter.symbol = "reuse";
      parameter.unit = "toggle";
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
    case kPitchBendRange:
      return pitchBendRange;
    case kKit:
      return kit;
    case kReuse:
      return reuse;
      
    default:
      return 0.0;
    }
  }

  // change drummer if necessary, send all current parameter depending on override option -- also update output parameter
  void updateDrummer() {
    // discard call if kit index is invalid (e.g. upon init override switch called before first kit is selected)
    if (kit >= 0) {
      drummer.switchTo(kit);
      // everything need to be handled at this level, meta kit only takes car of init
      drummer.setSamplerate(getSampleRate()  / 1000.0f);
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
    case kKit:
      if (value != kit) {
        kit = value;
        updateDrummer();
      }
      break;
    case kReuse:
      reuse = value;
      drummer.setReuse(reuse);

    default:
      break;
    }
  }

  // callbacks for processing MIDI
  void noteOn(uint8_t note, uint8_t velocity, uint8_t channel, uint32_t) {
    // filter event depending on selected channel
    if (channelInput == 0 or channelInput - 1 == channel) {
      drummer.noteOn(note, velocity, channel);
    }
  }

  void noteOff(uint8_t note, uint8_t channel, uint32_t) {
    // filter event depending on selected channel
    if (channelInput == 0 or channelInput - 1 == channel) {
      drummer.noteOff(note, channel);
    }
  }

  void pitchbend(uint8_t channel, float ratio, uint32_t) {
    // filter event depending on selected channel
    if (channelInput == 0 or channelInput - 1 == channel) {
      drummer.setPitchBend(ratio * pitchBendRange);
    }
  }

  void process(uint32_t chunkSize, uint32_t) {
    drummer.process(chunkSize, buffOut);
  }

  // Optional callback to inform synth about a sample rate change on the plugin side.
  void sampleRateChanged(double newSampleRate) override
  {
    drummer.setSamplerate(newSampleRate / 1000.0f);
  }

private:
  MetaDrummer drummer;

  // parameters
  int channelInput;
  int pitchBendRange;
  // init to impossible value to let first init happen
  int kit = -1;
  int reuse;

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Drummer);
};

Plugin *createPlugin() { return new Drummer(); }

END_NAMESPACE_DISTRHO
