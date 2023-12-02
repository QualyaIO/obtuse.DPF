
#ifndef DISTRHO_PLUGIN_INFO_H_INCLUDED
#define DISTRHO_PLUGIN_INFO_H_INCLUDED

#define DISTRHO_PLUGIN_NAME  "Sampler"
#define DISTRHO_PLUGIN_URI   "http://ullo.fr/botania/sampler"
#define DISTRHO_PLUGIN_CLAP_ID "botania.sampler"

#define DISTRHO_PLUGIN_NUM_INPUTS   0
#define DISTRHO_PLUGIN_NUM_OUTPUTS  1
#define DISTRHO_PLUGIN_WANT_MIDI_INPUT 1
#define DISTRHO_PLUGIN_IS_RT_SAFE   1
#define DISTRHO_PLUGIN_IS_SYNTH 1

enum Parameters {
    kChannelInput,

    kSample,
    kOverride,
    kLoop,
    kLoopStart,
    kLoopEnd,
    // output, to inform about sample current config
    kEffectiveLoop,
    kEffectiveLoopStart,
    kEffectiveLoopEnd,
    // number of samples here
    kSize,
    // TODO: add also default value?

    kParameterCount
};

#endif
