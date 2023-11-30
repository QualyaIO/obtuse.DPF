
#ifndef DISTRHO_PLUGIN_INFO_H_INCLUDED
#define DISTRHO_PLUGIN_INFO_H_INCLUDED

#define DISTRHO_PLUGIN_NAME  "Gate"
#define DISTRHO_PLUGIN_URI   "http://ullo.fr/botania/gate"
#define DISTRHO_PLUGIN_CLAP_ID "botania.gate"

// Note: CV input
#define DISTRHO_PLUGIN_NUM_INPUTS   0
#define DISTRHO_PLUGIN_NUM_OUTPUTS  0
#define DISTRHO_PLUGIN_WANT_MIDI_INPUT 1
#define DISTRHO_PLUGIN_WANT_MIDI_OUTPUT 1
#define DISTRHO_PLUGIN_IS_RT_SAFE   1

enum Parameters {
    kChannelInput, 
    kChannelOutput, 
    kDuration,

    kParameterCount
};

#endif
