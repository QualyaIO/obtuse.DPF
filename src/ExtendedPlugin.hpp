
#include "DistrhoPlugin.hpp"

class ExtendedPlugin : public Plugin {

public:

  // will forward to Plugin
  ExtendedPlugin(uint32_t parameterCount, uint32_t programCount, uint32_t stateCount) : Plugin(parameterCount, programCount, stateCount) {}

  // The following functions will be called upon encountering MIDI events, to be implemented by subclasses. channel: 0..15
  virtual void noteOn(uint8_t note, uint8_t velocity, uint8_t channel) {};
  virtual void noteOff(uint8_t note, uint8_t channel) {};
  // pitch bend from -2 to +2 semitones
  virtual void pitchbend(uint8_t chanel, float value) {};
  virtual void cc(uint8_t number, uint8_t value, uint8_t channel) {};
  // special CC
  virtual void sustain(uint8_t channel, bool flag) {};

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
	  noteOn(midiEvent.data[1], midiEvent.data[2], chan);
	}
	
	break;
	// note off
      case 128:
	if (midiEvent.size > 1) {
	  noteOff(midiEvent.data[1], chan);
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
	      sustain(chan, true);
	    } else {
	      sustain(chan, false);
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
	  float semitones = 0.0;
	  // compute semitones, for now will bend +/- one tone
	  // from 0 (-2 semitones) to 16383 (+2 semitones), 8192: no bend
	  if (pitchBend > 8192) {
	    semitones =  2.0 * (pitchBend - 8192) / (8191);
	  }
	  else if (pitchBend < 8192) {
	    semitones =  - 2.0 * (8192 - pitchBend) / (8192);
	  }
	  pitchbend(chan, semitones);
	}
	break;
      }
    }
  };

};
