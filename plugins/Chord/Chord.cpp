
#include "ExtendedPlugin.hpp"
#include "utils.h"

START_NAMESPACE_DISTRHO

// to sync with DSP
#define CHORD_NB_SCALES 20
#define CHORD_NB_CHORDS 6
// how many notes in chord and scale, to sync with DSP
#define CHORD_CHORD_SIZE 3
#define CHORD_MAX_SCALE_SIZE 12

// Wrapper for Chord.
// FIXME: scale sent only upon change in root or scale at the moment... change that?
// TODO: implement reset input
// TODO: ouput parameter for current chord?
// Note: send chord via MIDI, hence will change behavior of things like arp if used after, compared to plain DSP or VCV (e.g. use of note off will temporarily change number of active notes in arp). Will also note off / note off all 3 notes for each new chord, even if unchanged.
// Note: changing chord or scale midi channel will note off on pevious chan / note on on current chan pending chord or scale. 
// Note: changing root note or scale will output scale in MIDI, hence beware of spamming midi if automated
// Note: in this version chord change can also be triggered through midi, and any noteOn event on the corresponding MIDI channel.
// TODO: add reset input? (e.g. transport to beginning)
class Chord : public ExtendedPlugin {
public:
  // Note: do not care with default values since we will sent all parameters upon init
  Chord() : ExtendedPlugin(kParameterCount, 0, 0), root(60.0f) {
    utils_Tonnetz_process_init(context_processor);
    utils_Tonnetz_setRoot(context_processor, root);

    DISTRHO_SAFE_ASSERT_RETURN(
                               (CHORD_NB_SCALES  == utils_Tonnetz_getNbScales(context_processor)) &&
                               "Number of scales for chord different than expected (20)"
                               ,;
           );
    DISTRHO_SAFE_ASSERT_RETURN(
                               (CHORD_NB_SCALES  == utils_Tonnetz_getNbScales(context_processor)) &&
                               "Number of scales for chord different than expected (20)"
                               ,;
           );

    // init scale, scale will be init and sent upon first parameter sent
  }

protected:
  // metadata
  const char *getLabel() const override { return "BotaniaChord"; }
  const char *getDescription() const override {
    return "I generate chords things.";
  }
  const char *getMaker() const override { return "jfrey"; }
  const char *getLicense() const override { return "Custom"; }
  uint32_t getVersion() const override { return d_version(1,0,0); }
  int64_t getUniqueId() const override { 
    return d_cconst('B','C','R','D'); 
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
            port.hints   = kAudioPortIsCV | kCVPortIsOptional;
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
    case kChannelInput:
      parameter.hints = kParameterIsInteger | kParameterIsAutomatable;
      parameter.name = "Input MIDI channel for root";
      parameter.shortName = "root chan";
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
    case kChannelTriggerInput:
      parameter.hints = kParameterIsInteger | kParameterIsAutomatable;
      parameter.name = "Input MIDI channel for triggers";
      parameter.shortName = "trig chan";
      parameter.symbol = "channel";
      // using enum to explicit omni. 0 for omni and 16 channels, option to disable.
      parameter.enumValues.count = 18;
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
        values[17].label = "disable";
        values[17].value = 17;
      }
      // select default idx
      parameter.ranges.def = 17.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 17.0f;
      break;
    case kScale:
      parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
      parameter.name = "Scale";
      parameter.shortName = "Scale";
      parameter.symbol = "scale";
      parameter.enumValues.count = CHORD_NB_SCALES;
      parameter.enumValues.restrictedMode = true;
      {
        ParameterEnumerationValue* const values = new ParameterEnumerationValue[parameter.enumValues.count];
        parameter.enumValues.values = values;
        values[0].label = "all notes";
        values[0].value = 0;
        values[1].label = "diatonic ionian (major)";
        values[1].value = 1;
        values[2].label = "diatonic dorian";
        values[2].value = 2;
        values[3].label = "diatonic phrygian";
        values[3].value = 3;
        values[4].label = "diatonic lydian";
        values[4].value = 4;
        values[5].label = "diatonic mixolydian";
        values[5].value = 5;
        values[6].label = "diatonic aeolian (minor)";
        values[6].value = 6;
        values[7].label = "diatonic locrian";
        values[7].value = 7;
        values[8].label = "pentatonic major";
        values[8].value =  8;
        values[9].label = "pentatonic minor";
        values[9].value = 9;
        values[10].label = "melodic major";
        values[10].value = 10;
        values[11].label = "melodic minor";
        values[11].value = 11;
        values[12].label = "harmonic major";
        values[12].value = 12;
        values[13].label = "harmonic minor";
        values[13].value = 13;
        values[14].label = "hungarian major";
        values[14].value = 14;
        values[15].label = "hungarian minor";
        values[15].value = 15;
        values[16].label = "spanish heptatonic";
        values[16].value = 16;
        values[17].label = "flamenco";
        values[17].value = 17;
        values[18].label = "blues";
        values[18].value = 18;
        values[19].label = "enigmatic";
        values[19].value = 19;
      }
      // select default idx
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      // we must define max range to be able to select all values, 1 by default
      parameter.ranges.max = (float) (parameter.enumValues.count - 1);
      break;
    case kChord:
      parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
      parameter.name = "Chord";
      parameter.shortName = "Chord";
      parameter.symbol = "chord";
      parameter.enumValues.count = CHORD_NB_CHORDS;
      parameter.enumValues.restrictedMode = true;
      {
        ParameterEnumerationValue* const values = new ParameterEnumerationValue[parameter.enumValues.count];
        parameter.enumValues.values = values;
        values[0].label = "augmented";
        values[0].value = 0;
        values[1].label = "major";
        values[1].value = 1;
        values[2].label = "suspended 4";
        values[2].value = 2;
        values[3].label = "suspended 2";
        values[3].value = 3;
        values[4].label = "minor";
        values[4].value = 4;
        values[5].label = "diminished";
        values[5].value = 5;
      }
      // select default idx
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      // we must define max range to be able to select all values, 1 by default
      parameter.ranges.max = (float) (parameter.enumValues.count - 1);
      break;
    case kChordSpread:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Chord spread";
      parameter.shortName = "Chord sprd";
      parameter.symbol = "spread";
      parameter.ranges.def = 0.5f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;
    case kInvSpread:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Inversion spread";
      parameter.shortName = "Inv sprd";
      parameter.symbol = "spread";
      parameter.ranges.def = 0.5f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;
    case kJump:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Jump probability";
      parameter.shortName = "Jump proba";
      parameter.symbol = "p";
      parameter.ranges.def = 0.5f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;
    case kChannelChord:
      parameter.hints = kParameterIsInteger | kParameterIsAutomatable;
      parameter.name = "Chord MIDI channel";
      parameter.shortName = "chord chan";
      parameter.symbol = "channel";
      // 1-index here
      parameter.ranges.def = 1.0;
      parameter.ranges.min = 1.0f;
      parameter.ranges.max = 16.0f;
      break;
    case kChannelScale:
      parameter.hints = kParameterIsInteger | kParameterIsAutomatable;
      parameter.name = "Scale MIDI channel";
      parameter.shortName = "scale chan";
      parameter.symbol = "channel";
      // 1-index here
      parameter.ranges.def = 2.0;
      parameter.ranges.min = 1.0f;
      parameter.ranges.max = 16.0f;
      break;
    case kRoot:
      parameter.hints = kParameterIsInteger|kParameterIsOutput;
      parameter.name = "Root note";
      parameter.shortName = "note";
      parameter.symbol = "note";
      parameter.ranges.def = root;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 127.0f;
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
    case kChannelTriggerInput:
      return channelTriggerInput;
    case kScale:
      return scale;
    case kChord:
      return chord;
    case kChordSpread:
      return chordSpread;
    case kInvSpread:
      return invSpread;
    case kJump:
      return jump;
    case kRoot:
      return root;

    default:
      return 0.0;
    }
  }
  
  void setParameterValue(uint32_t index, float value) override {
    // FIXME: check up to which point function is repeatedly called from host even when value does not change
    switch (index) {
    case kChannelInput:
      channelInput = value;
      break;
    case kChannelTriggerInput:
      channelTriggerInput = value;
      break;
    case kScale:
      // retrig scale upon change
      sendScaleOff();
      scale = value;
      utils_Tonnetz_setScale(context_processor, value);
      updateScale();
      sendScaleOn();
      break;
    case kChord:
      chord = value;
      utils_Tonnetz_setChord(context_processor, value);
      break;
    case kChordSpread:
      chordSpread = value;
      utils_Tonnetz_setChordSpread(context_processor, float_to_fix(value));
      break;
    case kInvSpread:
      invSpread = value;
      utils_Tonnetz_setInversionSpread(context_processor, float_to_fix(value));
      break;
    case kJump:
      jump = value;
      utils_Tonnetz_setJump(context_processor, float_to_fix(value));
      break;
    case kChannelChord:
      // retrig chord upon channel change
      if (channelChord != value) {
        sendChordOff();
        channelChord = value;
        sendChordOn();
      }
      break;
    case kChannelScale:
      // retrig scale upon channel change
      if (channelScale != value) {
        sendScaleOff();
        channelScale = value;
        sendScaleOn();
      }
      break;
      // kRoot is output only, set through MIDI here, but just in case
    case kRoot:
      break;

    default:
      break;
    }
  }

  // callbacks for processing MIDI
  // changing root note, retrig scale
  void noteOn(uint8_t note, uint8_t, uint8_t channel, uint32_t frame) {
    // filter event depending on selected channel
    if (channelInput == 0 or channelInput - 1 == channel) {
      sendScaleOff(frame);
      root = note;
      utils_Tonnetz_setRoot(context_processor, note);
      updateScale();
      sendScaleOn(frame);
    }
    // same for triggering -- only if not triggered
    if (channelTriggerInput == 0 or channelTriggerInput - 1 == channel) {
      switchChord(frame);
    }
  }

  void sendChordOff(uint32_t frame=0) {
    for (int i = 0; i < CHORD_CHORD_SIZE; i++) {
      if (notes[i] >= 0) {
        sendNoteOff(notes[i], channelChord-1, frame);
      }
    }
  }

  void sendChordOn(uint32_t frame=0) {
    for (int i = 0; i < CHORD_CHORD_SIZE; i++) {
      if (notes[i] >= 0) {
        sendNoteOn(notes[i], 127, channelChord-1, frame);
      }
    }
  }

  void updateScale() {
    // which tones are active or not for current scale
    uint8_t rawScale[CHORD_MAX_SCALE_SIZE] = {false, false, false, false, false, false, false, false, false, false, false, false};
    int scaleId = utils_Tonnetz_getScaleId(context_processor);
    utils_Tonnetz_getScale(scaleId, rawScale);
    // retrieve root from DSP, just to be sure (there might be some discrepancy upon init).
    // TODO: maybe check for discrepancy?
    int trueRoot = utils_Tonnetz_getRoot(context_processor);
    int k = 0;
    // fill the scale with selected notes, related to root
    for (int i = 0; i < CHORD_MAX_SCALE_SIZE; i++) {
      if (rawScale[i]) {
        scaleNotes[k] = trueRoot + i;
        // mitigate issue with high root, down one octave, should match how notes within chords are also one octave down in DSP tonnetz
        if(scaleNotes[k] > 127) {
          scaleNotes[k] = scaleNotes[k] - 12;
        }
        k++;
      }
    }
    scaleSize = k;
  }

  void sendScaleOff(uint32_t frame=0) {
    for (int i = 0; i < CHORD_MAX_SCALE_SIZE && i < scaleSize; i++) {
      if (scaleNotes[i] >= 0) {
        sendNoteOff(scaleNotes[i], channelScale-1, frame);
      }
    }
  }

  void sendScaleOn(uint32_t frame=0) {
    for (int i = 0; i < CHORD_MAX_SCALE_SIZE && i < scaleSize; i++) {
      if (scaleNotes[i] >= 0) {
        sendNoteOn(scaleNotes[i], 127, channelScale-1, frame);
      }
    }
  }

  void switchChord(uint32_t frame=0) {
    // turn off previous chord
    sendChordOff(frame);
    // draw and retrieve chord
    utils_Tonnetz_process(context_processor);
    notes[0] = utils_Tonnetz_process_ret_0(context_processor);
    notes[1] = utils_Tonnetz_process_ret_1(context_processor);
    notes[2] = utils_Tonnetz_process_ret_2(context_processor);
    // send chord
    sendChordOn(frame);
  }

  void process(uint32_t chunkSize, uint32_t frame) {
    for (unsigned int i = 0; i < chunkSize; i++) {
      // threshold 0.1 for trigger, new chord upon trigger
      if (fix_to_float(buffIn[i]) >= 0.1 and !trigerring) {
        trigerring = true;
        // order to switch chord at the exact frame
        switchChord(frame + i);
      }
      // nothing to be done when trigger in finished
      else if (fix_to_float(buffIn[i]) < 0.1 and trigerring) {
        trigerring = false;
      }
    }
  }

private:
  utils_Tonnetz_process_type context_processor;
  // currently receive a trigger
  bool trigerring = false;
  // chord notes
  int notes[CHORD_CHORD_SIZE] = {-1, -1, -1};
  // selected scale
  int scaleNotes[CHORD_MAX_SCALE_SIZE] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
  // actual number of notes for the scale
  int scaleSize = 0;

  int channelInput;
  int channelTriggerInput;
  float scale;
  float chord;
  float chordSpread;
  float invSpread;
  float jump;
  // same as in DSP
  float root;
  // where to send notes
  int channelChord;
  int channelScale;

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Chord);
};

Plugin *createPlugin() { return new Chord(); }

END_NAMESPACE_DISTRHO
