
#ifndef DISTRHO_PLUGIN_INFO_H_INCLUDED
#define DISTRHO_PLUGIN_INFO_H_INCLUDED

#define DISTRHO_PLUGIN_NAME  "ObtuseSampler"
#define DISTRHO_PLUGIN_URI   "https://qualya.io/obtuse/sampler"
#define DISTRHO_PLUGIN_CLAP_ID "obtuse.sampler"

#define DISTRHO_PLUGIN_NUM_INPUTS   0
#define DISTRHO_PLUGIN_NUM_OUTPUTS  1
#define DISTRHO_PLUGIN_MIDI_MPE 1
#define DISTRHO_PLUGIN_WANT_MIDI_INPUT 1
#define DISTRHO_PLUGIN_IS_RT_SAFE   1
#define DISTRHO_PLUGIN_IS_SYNTH 1

enum Parameters {
    kChannelInput,
    kPitchBendRange,

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
    // reuse flag for voice
    kReuse,

    kParameterCount
};

#endif
