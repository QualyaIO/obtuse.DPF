
#include "ExtendedPluginFP.hpp"
#include "effectsXL.h"
#include "PluginUtils.h"

START_NAMESPACE_DISTRHO

// over how long (in ms) activity is computed
// Note: for efficiency, might be a greater time, will update at most once per buffer (parameters won't update faster anyhow)
#define TIME_ACTIVITY 20

// Wrapper for combFB
// NOTE: output not guaranteed to be kept in -1..1 range, use Saturator after
// TODO: expose limits due to buffer size?
// FIXME: audio glitches upon change in delay while playing
class CombFB : public ExtendedPlugin {
public:
  CombFB() : ExtendedPlugin(kParameterCount, 0, 0) {
    effectsXL_CombFB_process_init(context_processor);
    effectsXL_CombFB_setSamplerate(context_processor, float_to_fix((float)getSampleRate() / 1000.0f));
    nbActivityFrames = getSampleRate() * (TIME_ACTIVITY/1000.0);
    // give it a default value, will be actually once per buffer
    if (nbActivityFrames == 0) {
      nbActivityFrames = 1;
    }
  }

protected:
  // metadata
  const char *getLabel() const override { return "ObtuseCombFB"; }
  const char *getDescription() const override {
    return "I comb backward filter things.";
  }
  const char *getMaker() const override { return "Qualya"; }
  uint32_t getVersion() const override { return d_version(0,1,0); }
  int64_t getUniqueId() const override { 
    return d_cconst('O','C','F','B');
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
    case kDecay:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Decay";
      parameter.shortName = "Dec";
      parameter.symbol = "decay";
      parameter.unit = "ratio";
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
   // actually max delay will depend on buffer size, with XL 16384 buffer and 44100 fs it's only 371ms -- see below
      parameter.ranges.def = params[index].def;
      parameter.ranges.min = params[index].min;
      parameter.ranges.max = params[index].max;
      break;
    case kMaxDelay:
      parameter.hints = kParameterIsOutput;
      parameter.name = "Maximum Delay";
      parameter.shortName = "max del";
      parameter.symbol = "maxdelay";
      parameter.unit = "ms";
      // here we actually compute actual value
      parameter.ranges.def = effectsXL_Buffer_bufferLargeSize() / getSampleRate() * 1000;
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
    case kDecay:
      return decay;
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
    case kDecay:
      decay = value;
      effectsXL_CombFB_setDecay(context_processor, float_to_fix(decay));
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
        effectsXL_CombFB_process_bufferTo(context_processor, chunkSize, buffIn, buffOut);
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
    effectsXL_CombFB_setSamplerate(context_processor, float_to_fix((float)newSampleRate / 1000.0f));
    // apply again delay because in the DSP ultimately a number of sample is used
    updateDelay();
    // update info about maximum delay
    setParameterValue(kMaxDelay, effectsXL_Buffer_bufferLargeSize() / newSampleRate * 1000);
    // now activity window computation will change as well
    nbActivityFrames = newSampleRate * (TIME_ACTIVITY/1000.0);
    // failsafe
    if (nbActivityFrames == 0) {
      nbActivityFrames = 1;
    }
  }
  
private:
  effectsXL_CombFB_process_type context_processor;

  // parameters
  float dryWet;
  float decay;
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
      effectsXL_CombFB_setDelayms(context_processor, float_to_fix(realDelay));
  }

  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CombFB);
};

Plugin *createPlugin() { return new CombFB(); }

END_NAMESPACE_DISTRHO
