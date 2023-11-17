
#include "ExtendedPlugin.hpp"
#include "utils.h"
#include <cassert>

START_NAMESPACE_DISTRHO

// to sync with vult code
#define ARP_MAX_NOTES 16

// Wrapper for Arp. Up, down, etc. will refer to note order. Duplicated note brings it to newest.
// Note: In this version, the duration of the trigger input will be the duration of noteOn / noteOff MIDI events.
// TODO: support MIDI sustain at this level, and/or add an "hold" option?
class Arp : public ExtendedPlugin {
public:
  // Note: do not care with default values since we will sent all parameters upon init
  Arp() : ExtendedPlugin(kParameterCount, 0, 0) {
    utils_Arp_process_init(context_processor);
    utils_Gate_list_init(context_list);
    // will init notes to -1
    updateNotes();
    // make sure we don't do anything rash - but compiler will catch discrepancy also while setNotes
    assert(
           (utils_Arp_getMaxNbNotes(context_processor) == ARP_MAX_NOTES) &&
           "Number of notes held in arp differs from expected (16)"
           );
  }

protected:
  // metadata
  const char *getLabel() const override { return "BotaniaArp"; }
  const char *getDescription() const override {
    return "I arp things.";
  }
  const char *getMaker() const override { return "jfrey"; }
  const char *getLicense() const override { return "Custom"; }
  uint32_t getVersion() const override { return d_version(1,0,0); }
  int64_t getUniqueId() const override { 
    return d_cconst('B','A','R','P'); 
  }

  // ports
  void initAudioPort(bool input, uint32_t index, AudioPort& port) override
  {
    if (input)
      {
        switch (index)
          {
          case 0:
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
    case kMode:
      parameter.hints = kParameterIsAutomatable|kParameterIsInteger;
      parameter.name = "Mode";
      parameter.shortName = "Mode";
      parameter.symbol = "mode";
      // we want to make sure we are on par with API
      parameter.enumValues.count = 6;
      assert(
             (parameter.enumValues.count  == Processor_arp_getNbModes(context_processor) - 1) &&
             "Number of modes for arp different than expected (6)"
             );
      parameter.enumValues.restrictedMode = true;
      {
        ParameterEnumerationValue* const values = new ParameterEnumerationValue[parameter.enumValues.count];
        parameter.enumValues.values = values;
        values[0].label = "up";
        values[0].value = 0;
        values[1].label = "down";
        values[1].value = 1;
        values[2].label = "up-down no repeat";
        values[2].value = 2;
        values[3].label = "up-down repeat";
        values[3].value = 3;
        values[4].label = "down-up no repeat";
        values[4].value = 4;
        values[5].label = "down-up repeat";
        values[5].value = 5;
      }
      // select default idx
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      // we must define max range to be able to select all values, 1 by default
      parameter.ranges.max = (float) (parameter.enumValues.count - 1);
      break;
    case kRandNotes:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Random notes amount";
      parameter.shortName = "Rand notes";
      parameter.symbol = "amount";
      parameter.ranges.def = 0.0f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;
    case kRandomize:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Randomize probability";
      parameter.shortName = "Randomize";
      parameter.symbol = "p";
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

    case kMode:
      return mode;
    case kRandNotes:
      return randNotes;
    case kRandomize:
      return randomize;

    default:
      return 0.0;
    }
  }
  
  void setParameterValue(uint32_t index, float value) override {
    // FIXME: check up to which point function is repeatedly called from host even when value does not change
    switch (index) {
    case kMode:
      mode = value;
      utils_Arp_setMode(context_processor, value);
      break;
    case kRandNotes:
      randNotes = value;
      utils_Arp_setPRandomNotes(context_processor, float_to_fix(value));
      break;
    case kRandomize:
      randomize = value;
      utils_Arp_setPRandomize(context_processor, float_to_fix(value));
      break;

    default:
      break;
    }
  }

  // callbacks for processing MIDI
  // keep track of xx last active notes, duplicate no matter the channel will reset position. Using gate's list
  void noteOn(uint8_t note, uint8_t, uint8_t channel) {
    // turn off previous note, if any
    noteOff(note, channel);
    // add note to list
    utils_Gate_push(context_list, note);
    // update arp
    updateNotes();
  }

  // remove note from active list
  void noteOff(uint8_t note, uint8_t) {
    // Do we have the note already?
    int idx = utils_Gate_search(context_list, note);
    // If so, remove it, and will update arp
    if (idx >= 0) {
      utils_Gate_delete(context_list, idx);
      updateNotes();
    }
  }

  void process(uint32_t chunkSize, uint32_t frame) {
    static int lastNote = -1;
    for (unsigned int i = 0; i < chunkSize; i++) {
      // threshold 0.1 for trigger, advance note upon trigger
      if (fix_to_float(buffIn[i]) >= 0.1 and !trigerring) {
        trigerring = true;
        // advance arp
        lastNote  = utils_Arp_process(context_processor);
        d_stdout("trig note: %d", lastNote);
        // send MIDI
        if (lastNote >= 0) {
          sendNoteOn(lastNote, 127, 0, frame+i);
        }
      }
      // turn off note
      else if (fix_to_float(buffIn[i]) < 0.1 and trigerring) {
        trigerring = false;
        if (lastNote >= 0) {
          sendNoteOff(lastNote, 0, frame+i);
        }
      }
    }
  }

private:
  utils_Arp_process_type context_processor;
  utils_Gate_list_type context_list;
  // currently receive a trigger
  bool trigerring = false;

  float mode;
  float randNotes;
  float randomize;

  // re-compute active notes and send to arp
  void updateNotes() {
    d_stdout("update notes");
    // retrieve number of notes currently held and how much can fit in arp
    int nbNotes = utils_Gate_getListSize(context_list);
    // we want most recent notes for arp
    int listStartIdx = nbNotes - ARP_MAX_NOTES;
    // at most number of elements, of course
    if (listStartIdx < 0) {
      listStartIdx = 0;
    }
    // now build array to be sent to arp from there
    int arpNotes[ARP_MAX_NOTES];
    int n = 0;
    while (n < nbNotes && n < ARP_MAX_NOTES) {
      arpNotes[n] = utils_Gate_peek(context_list, listStartIdx + n);
      n++;
    }
    // finish with -1
    while (n < ARP_MAX_NOTES) {
      arpNotes[n] = -1;
      n++;
    }

    // debug
    for (int i = 0; i < ARP_MAX_NOTES; i++) {
      d_stdout("%d: %d", i, arpNotes[i]);
    }
    
    utils_Arp_setNotes(context_processor, arpNotes);
  }

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Arp);
};

Plugin *createPlugin() { return new Arp(); }

END_NAMESPACE_DISTRHO
