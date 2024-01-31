
#ifndef DISTRHO_PLUGIN_INFO_H_INCLUDED
#define DISTRHO_PLUGIN_INFO_H_INCLUDED

#define DISTRHO_PLUGIN_NAME  "Drummer"
#define DISTRHO_PLUGIN_URI   "http://ullo.fr/botania/drummer"
#define DISTRHO_PLUGIN_CLAP_ID "botania.drummer"

#define DISTRHO_PLUGIN_NUM_INPUTS   0
#define DISTRHO_PLUGIN_NUM_OUTPUTS  1
#define DISTRHO_PLUGIN_MIDI_MPE 1
#define DISTRHO_PLUGIN_WANT_MIDI_INPUT 1
#define DISTRHO_PLUGIN_IS_RT_SAFE   1
#define DISTRHO_PLUGIN_IS_SYNTH 1

enum Parameters {
    kChannelInput,
    kPitchBendRange,
    kKit,
    // reuse flag for voice
    kReuse,

    kParameterCount
};

#endif
