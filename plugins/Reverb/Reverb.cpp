
#include "ExtendedPluginFP.hpp"
#include "effects.h"
#include "PluginUtils.h"

START_NAMESPACE_DISTRHO

// over how long (in ms) activity is computed
// Note: for efficiency, might be a greater time, will update at most once per buffer (parameters won't update faster anyhow)
#define TIME_ACTIVITY 20

// Wrapper for reverb
// NOTE: output not guaranteed to be kept in -1..1 range, use Saturator after
// NOTE: using regular buffer size, as in VCV, thus limited max delay
// TODO: expose limits due to buffer size?
// FIXME: audio glitches upon change in delay while playing. Also be wary of small delays (e.g. < 10ms).
class Reverb : public ExtendedPlugin {
public:
  Reverb() : ExtendedPlugin(kParameterCount, 0, 0) {
    effects_Reverb_process_init(context_processor);
    effects_Reverb_setSamplerate(context_processor, float_to_fix((float)getSampleRate() / 1000.0f));
  }

protected:
  // metadata
  const char *getLabel() const override { return "ObtuseReverb"; }
  const char *getDescription() const override {
    return "I comb reverb things.";
  }
  const char *getMaker() const override { return "Qualya"; }
  uint32_t getVersion() const override { return d_version(0,1,0); }
  int64_t getUniqueId() const override { 
    return d_cconst('O','R','V','B');
  }

  // params
  void initParameter (uint32_t index, Parameter& parameter) override {
    // TODO: should we retrieve default from actual value (after init)?
    switch (index) {
    case kDryWet:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Dry/Wet";
      parameter.shortName = "dw";
      parameter.symbol = "drywet";
      parameter.unit = "ratio";
      parameter.ranges.def = params[index].def;
      parameter.ranges.min = params[index].min;
      parameter.ranges.max = params[index].max;
      break;
    case kReverb:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Reverberation time (T60)";
      parameter.shortName = "Reverb";
      parameter.symbol = "reverberationtime";
      parameter.unit = "seconds";
      parameter.ranges.def = params[index].def;
      parameter.ranges.min = params[index].min;
      parameter.ranges.max = params[index].max;
      break;
    case kDelay:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Delay";
      parameter.shortName = "del";
      parameter.symbol = "delay";
      parameter.unit = "ms";
      // actually max delay will depend on buffer size, with medium 2048 buffer and 44100 fs it's only 46ms. Also min delay is capped to avoid glitches -- see below
      parameter.ranges.def = params[kActivity].def;
      parameter.ranges.min = params[kActivity].min;
      parameter.ranges.max = params[kActivity].max;
      break;
    case kMaxDelay:
      parameter.hints = kParameterIsOutput;
      parameter.name = "Maximum Delay";
      parameter.shortName = "max del";
      parameter.symbol = "maxdelay";
      parameter.unit = "ms";
      // here we actually compute actual value
      parameter.ranges.def = effects_Buffer_bufferLargeSize() / getSampleRate() * 1000;
      parameter.ranges.min = params[kMaxDelay].min;
      parameter.ranges.max = params[kMaxDelay].max;
      break;
     case kActivity:
      parameter.hints = kParameterIsOutput;
      parameter.name = "Activity";
      parameter.shortName = "activity";
      parameter.symbol = "activity";
      parameter.unit = "ratio";
      parameter.ranges.def = params[kActivity].def;
      parameter.ranges.min = params[kActivity].min;
      parameter.ranges.max = params[kActivity].max;
      break;
    default:
      break;
    }

    // effectively set parameter
    setParameterValue(index, parameter.ranges.def);
  }

  float getParameterValue(uint32_t index) const override {
    switch (index) {
    case kDryWet:
      return dryWet;
    case kReverb:
      return reverb;
    case kDelay:
      return delay;
    case kMaxDelay:
      return maxDelay;
    case kActivity:
      return activity;
    default:
      return 0.0;
    }
  }
 
  void setParameterValue(uint32_t index, float value) override {
    // FIXME: check up to which point function is repeatedly called from host even when value does not change
    switch (index) {
    case kDryWet:
      dryWet = value;
      break;
    case kReverb:
      reverb = value;
      effects_Reverb_setReverbTime(context_processor, float_to_fix(reverb));
      break;
    case kDelay:
      delay = value;
      updateDelay();
      break;
    case kMaxDelay:
      maxDelay = value;
      break;
    case kActivity:
      activity = value;
      break;
    default:
      break;
    }
  }

  // we take care ourselves of the main loop for efficient dry/wet (we don't have MIDI)
  void run(const float** inputs, float** outputs, uint32_t frames) override {
    const float *const in = inputs[0];
    float *const out = outputs[0];

    // failsafe
    if (in == NULL || out == NULL) {
      return;
    }

    // nothing to do if completely dry
    if (dryWet <= 0.0) {
      for (uint32_t i = 0; i < frames; i++) {
        out[i] = in[i];
        cumulatedActivity += out[i] * out[i]; 
      }
    }
    // process and mix
    else {
      // we will process in chunks, position of current frame
      uint32_t k = 0;
      while (k < frames) {
        // enough frames left for whole buffer or only leftovers?
        uint32_t chunkSize = ((frames - k) >= BUFFER_SIZE )?BUFFER_SIZE:(frames - k);
        // copy to input buffer
        if (in != NULL) {
          for (uint32_t i = 0; i < chunkSize; i++) {
            buffIn[i] = float_to_fix(in[k+i]);
          }
        }
        // run DSP
        effects_Reverb_process_bufferTo(context_processor, chunkSize, buffIn, buffOut);
        // copy to output buffer, with dry/wet
        if (out != NULL) {
          for (uint32_t i = 0; i < chunkSize; i++) {
            out[k+i] = (1 - dryWet) * in[k+i] + dryWet * fix_to_float(buffOut[i]);
            // gather output for activity
            cumulatedActivity += out[k+i] * out[k+i];
          }
        }
        // advance
        k += chunkSize;
      }
    }

    nbActivity = nbActivity + frames;
    // time to update activity
    if (nbActivity >= nbActivityFrames) {
      // average and one final squared root for RMS
      activity = pow(cumulatedActivity / nbActivity, 0.5) ;
      // clamp 0..1
      activity = activity > 1.0 ? 1.0 : activity;
      activity = activity < 0.0 ? 0.0 : activity;
      // reset
      cumulatedActivity = 0;
      nbActivity = 0;
    }
  }

  // Optional callback to inform synth about a sample rate change on the plugin side.
  void sampleRateChanged(double newSampleRate) override
  {
    effects_Reverb_setSamplerate(context_processor, float_to_fix((float)newSampleRate / 1000.0f));
    // apply again delay because in the DSP ultimately a number of sample is used
    updateDelay();
    // update info about maximum delay
    setParameterValue(kMaxDelay, effects_Buffer_bufferLargeSize() / newSampleRate * 1000);
    // now activity window computation will change as well
    nbActivityFrames = newSampleRate * (TIME_ACTIVITY/1000.0);
    // failsafe
    if (nbActivityFrames == 0) {
      nbActivityFrames = 1;
    }
  }
  
private:
  effects_Reverb_process_type context_processor;

  // parameters
  float dryWet;
  float reverb;
  float activity;
  // init with some value since it will be used upon sample rate change
  float delay = 10.0;
  float maxDelay = 10.0;
  // how many frames it takes to update activity
  unsigned int nbActivityFrames = 1;
  // how many frames we are in
  unsigned int nbActivity = 0;
  double cumulatedActivity = 0;

  void updateDelay() {
      // HOTFIX: make sure we do not overflow fixed float
      float realDelay = delay;
      if (getSampleRate() > 0 and realDelay > (32767.0f / getSampleRate()) * 1000) {
        realDelay = (32767.0f / getSampleRate()) * 1000;
      }
      // HOTFIX: make sure the delay is high enough to avoid ugly glitches. Very dependent on current DSP implementation, with 345 samples in biggest line.
      else if (getSampleRate() > 0 and realDelay < (346.0f / getSampleRate()) * 1000) {
        realDelay = (346.0f / getSampleRate()) * 1000;
      }
      effects_Reverb_setDelayms(context_processor, float_to_fix(realDelay));
  }

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Reverb);
};

Plugin *createPlugin() { return new Reverb(); }

END_NAMESPACE_DISTRHO
