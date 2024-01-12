
#ifndef EXTENDED_PLUGIN_HPP
#define EXTENDED_PLUGIN_HPP

#include "DistrhoPlugin.hpp"
#include "vultin.h"

// chunk size to process audio
// to sync with vult code
#define BUFFER_SIZE 128


// wrapper to factorize code between plugins. Consider at the moment at most one input and one output
// TODO: option to process MIDI event with frame-perfect accuracy? (at the moment precision of the call is chunk size)
class ExtendedPlugin : public Plugin {

public:

  // will forward to Plugin
  ExtendedPlugin(uint32_t parameterCount, uint32_t programCount, uint32_t stateCount) : Plugin(parameterCount, programCount, stateCount) {
    // init buffer
    for (int i = 0; i < BUFFER_SIZE; i++) {
      buffIn[i] = float_to_fix(0.0);
      buffOut[i] = float_to_fix(0.0);
    }

  }

protected:
  // to be filled by sub-class
  fix16_t buffIn[BUFFER_SIZE];
  fix16_t buffOut[BUFFER_SIZE];

  const char *getLicense() const override { return "http://ullo.fr/botania/custom_licence"; }

  // The following functions will be called upon encountering MIDI events, to be implemented by subclasses. channel: 0..15. frame: frame number within the buffer of the MIDI event
  virtual void noteOn(uint8_t /*note*/, uint8_t /*velocity*/, uint8_t /*channel*/, uint32_t /*frame*/) {};
  virtual void noteOff(uint8_t /*note*/, uint8_t /*channel*/, uint32_t /*frame*/) {};
  // pitch bend, ratio between -1 and 1, to be converted by plugin in semitones
  virtual void pitchbend(uint8_t /*chanel*/, float /*ratio*/, uint32_t /*frame*/) {};
  virtual void cc(uint8_t /*number*/, uint8_t /*value*/, uint8_t /*channel*/, uint32_t /*frame*/) {};
  // special CC
  virtual void sustain(uint8_t /*channel*/, bool /*flag*/, uint32_t /*frame*/) {};

  // deal with MIDI input
  void processMidiEvent(const MidiEvent midiEvent) {
    // only process regular midi even
    if (midiEvent.size > 1 and midiEvent.size <= 4) {
      // channel and type of event on first value
      uint8_t chan = midiEvent.data[0] & 0x0F;
      uint8_t type = midiEvent.data[0] & 0xF0;
      switch(type) {
	
	// note on
      case 144:
	if (midiEvent.size > 2) {
	  noteOn(midiEvent.data[1], midiEvent.data[2], chan, midiEvent.frame);
	}
	
	break;
	// note off
      case 128:
	if (midiEvent.size > 1) {
	  noteOff(midiEvent.data[1], chan, midiEvent.frame);
	}
	break;
	
	// cc
      case 176:
	if (midiEvent.size > 3) {
	  // cc number and then value
	  uint8_t cc = midiEvent.data[1];
	  uint8_t value = midiEvent.data[2];
	  switch(cc) {
	    // sustain
	  case 64:
	    if (value >= 64) {
	      sustain(chan, true, midiEvent.frame);
	    } else {
	      sustain(chan, false, midiEvent.frame);
	    }
	    break;
	  }
	}
	break;
	
	// pitch bend
	// data: pitchbend value = ev.buffer[1] | (ev.buffer[2] << 7);
      case 224:
	if (midiEvent.size > 2) {
	  // retrieve full value
	  int pitchBend = midiEvent.data[1] | (midiEvent.data[2] << 7);
	  float ratio = 0.0;
	  // from 0 (-1 ratio) to 16383 (+1 ratio), 8192: no bend
	  if (pitchBend > 8192) {
	    ratio =  1.0 * (pitchBend - 8192) / (8191);
	  }
	  else if (pitchBend < 8192) {
	    ratio =  -1.0 * (8192 - pitchBend) / (8192);
	  }
	  pitchbend(chan, ratio, midiEvent.frame);
	}
	break;
      }
    }
  };

#if DISTRHO_PLUGIN_WANT_MIDI_OUTPUT
  // output noteOn event
  // note, velocity, channel: as per usual MIDI
  // frame: frame of the event in the buffer
  // TODO: sanitize frame
  void sendNoteOn(uint8_t note, uint8_t velocity=127, uint8_t channel=0, uint32_t frame=0) {
    // sanitize
    channel = channel & 0x0F;
    // code for note on
    uint8_t type = 144;
    // build event
    MidiEvent midiEvent;
    midiEvent.frame = frame;
    midiEvent.size = 3;
    midiEvent.data[0] = type + channel;
    midiEvent.data[1] = note;
    midiEvent.data[2] = velocity;
    writeMidiEvent(midiEvent);
  }

  // output noteOff event
  // note, and channel: as per usual MIDI (velocity set to 0)
  // frame: frame of the event in the buffer
  void sendNoteOff(uint8_t note, uint8_t channel=0, uint32_t frame=0) {
    // sanitize
    channel = channel & 0x0F;
    // code for note on
    uint8_t type = 128;
    // build event
    MidiEvent midiEvent;
    midiEvent.frame = frame;
    midiEvent.size = 3;
    midiEvent.data[0] = type + channel;
    midiEvent.data[1] = note;
    midiEvent.data[2] = 0;
    writeMidiEvent(midiEvent);
  }
#endif // DISTRHO_PLUGIN_WANT_MIDI_OUTPUT

  // should read from buffIn and write to buffOut, and process a specific amount of samples, with reference of the starting frame
  virtual void process(uint32_t /*nbSamples*/, uint32_t /*frame*/) {};


#if DISTRHO_PLUGIN_WANT_MIDI_INPUT

  // only at most one input and/or one output
  void run(const float**
	   // fighting unused parameters...
#if DISTRHO_PLUGIN_NUM_INPUTS > 0
	   inputs
#endif // DISTRHO_PLUGIN_NUM_INPUTS > 0
	   , float**
#if DISTRHO_PLUGIN_NUM_OUTPUTS > 0
	   outputs
#endif // DISTRHO_PLUGIN_NUM_OUTPUTS > 0
	   , uint32_t frames,
	   const MidiEvent* midiEvents, uint32_t midiEventCount) override {

    // deal with audio I/O
#if DISTRHO_PLUGIN_NUM_INPUTS > 0
    const float *const in = inputs[0];
#endif // DISTRHO_PLUGIN_NUM_INPUTS > 0
#if DISTRHO_PLUGIN_NUM_OUTPUTS > 0
    float *const out = outputs[0];
#endif // DISTRHO_PLUGIN_NUM_OUTPUTS > 0

    // we will process in chunks, position of current frame
    uint32_t k = 0;
    // current midi 
    uint32_t midiEventNum = 0;
    while (k < frames) {
      // enough frames left for whole buffer or only leftovers?
      uint32_t chunkSize = ((frames - k) >= BUFFER_SIZE )?BUFFER_SIZE:(frames - k);

      // do we have any MIDI event to process before this chunk?
      while (midiEventNum < midiEventCount) {
        MidiEvent ev = midiEvents[midiEventNum];
        if (ev.frame <= k) {
          processMidiEvent(ev);
          midiEventNum++;
        }
        else {
          break;
        }
      }

      // copy to input buffer
#if DISTRHO_PLUGIN_NUM_INPUTS > 0
      // we could have NULL pointer, e.g. if asked for CV but not supported
      if (in != NULL) {
	for (uint32_t i = 0; i < chunkSize; i++) {
	  buffIn[i] = float_to_fix(in[k+i]);
	}
      }
#endif // DISTRHO_PLUGIN_NUM_INPUTS > 0
      // let subclass output buffer
      process(chunkSize, k);
      // copy to output buffer
#if DISTRHO_PLUGIN_NUM_OUTPUTS > 0
      if (out != NULL) {
	for (uint32_t i = 0; i < chunkSize; i++) {
	  out[k+i] = fix_to_float(buffOut[i]);
	}
      }
#endif // DISTRHO_PLUGIN_NUM_OUTPUTS > 0
      // advance
      k += chunkSize;
    }

    // MIDI event remaining
    while (midiEventNum < midiEventCount) {
      MidiEvent ev = midiEvents[midiEventNum];
      processMidiEvent(ev);
      midiEventNum++;
    }

  }

#else // DISTRHO_PLUGIN_WANT_MIDI_INPUT

  // only at most one input and/or one output 
  void run(const float**
#if DISTRHO_PLUGIN_NUM_INPUTS > 0
	   inputs
#endif // DISTRHO_PLUGIN_NUM_INPUTS > 0
	   , float**
#if DISTRHO_PLUGIN_NUM_OUTPUTS > 0
	   outputs
#endif // DISTRHO_PLUGIN_NUM_OUTPUTS > 0
	   , uint32_t frames) override {

    // deal with audio I/O
#if DISTRHO_PLUGIN_NUM_INPUTS > 0
    const float *const in = inputs[0];
#endif // DISTRHO_PLUGIN_NUM_INPUTS > 0
#if DISTRHO_PLUGIN_NUM_OUTPUTS > 0
    float *const out = outputs[0];
#endif // DISTRHO_PLUGIN_NUM_OUTPUTS > 0

    // we will process in chunks, position of current frame
    uint32_t k = 0;
    while (k < frames) {
      // enough frames left for whole buffer or only leftovers?
      uint32_t chunkSize = ((frames - k) >= BUFFER_SIZE )?BUFFER_SIZE:(frames - k);
      // copy to input buffer
#if DISTRHO_PLUGIN_NUM_INPUTS > 0
      if (in != NULL) {
	for (uint32_t i = 0; i < chunkSize; i++) {
	  buffIn[i] = float_to_fix(in[k+i]);
	}
      }
#endif // DISTRHO_PLUGIN_NUM_INPUTS > 0
      // let subclass output buffer
      process(chunkSize, k);
      // copy to output buffer
#if DISTRHO_PLUGIN_NUM_OUTPUTS > 0
      if (out != NULL) {
	for (uint32_t i = 0; i < chunkSize; i++) {
	  out[k+i] = fix_to_float(buffOut[i]);
	}
      }
#endif // DISTRHO_PLUGIN_NUM_OUTPUTS > 0
      // advance
      k += chunkSize;
    }

  }

#endif // DISTRHO_PLUGIN_WANT_MIDI_INPUT

  
};

#endif // EXTENDED_PLUGIN_HPP
