
#include "ExtendedPlugin.hpp"
#include "effects.h"

START_NAMESPACE_DISTRHO

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
  const char *getLabel() const override { return "BotaniaReverb"; }
  const char *getDescription() const override {
    return "I comb reverb things.";
  }
  const char *getMaker() const override { return "jfrey"; }
  const char *getLicense() const override { return "Custom"; }
  uint32_t getVersion() const override { return d_version(1,0,0); }
  int64_t getUniqueId() const override { 
    return d_cconst('B','R','V','B'); 
  }

  // params
  void initParameter (uint32_t index, Parameter& parameter) override {
    // TODO: should we retrieve default from actual value (after init)?
    switch (index) {
    case kDryWet:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Dry/Wet";
      parameter.shortName = "dw";
      parameter.symbol = "ratio";
      parameter.ranges.def = 0.5f;
      parameter.ranges.min = 0.0f;
      parameter.ranges.max = 1.0f;
      break;
    case kReverb:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Reverberation time (T60)";
      parameter.shortName = "Reverb";
      parameter.symbol = "seconds";
      parameter.ranges.def = 10.0f;
      parameter.ranges.min = 0.001f;
      parameter.ranges.max = 60.0f;
      break;
    case kDelay:
      parameter.hints = kParameterIsAutomatable;
      parameter.name = "Delay";
      parameter.shortName = "del";
      parameter.symbol = "ms";
      // actually max delay will depend on buffer size, with medium 2048 buffer and 44100 fs it's only 46ms. Also min delay is capped to avoid glitches.
      parameter.ranges.def = 50.0f;
      parameter.ranges.min = 1.0f;
      parameter.ranges.max = 100.0f;
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
          }
        }
        // advance
        k += chunkSize;
      }
    }
  }

  // Optional callback to inform synth about a sample rate change on the plugin side.
  void sampleRateChanged(double newSampleRate) override
  {
    effects_Reverb_setSamplerate(context_processor, float_to_fix((float)newSampleRate / 1000.0f));
    // apply again delay because in the DSP ultimately a number of sample is used
    updateDelay();
  }
  
private:
  effects_Reverb_process_type context_processor;

  // parameters
  float dryWet;
  float reverb;
  // init with some value since it will be used upon sample rate change
  float delay = 10.0;

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
