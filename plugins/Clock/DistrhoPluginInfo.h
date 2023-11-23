
#ifndef DISTRHO_PLUGIN_INFO_H_INCLUDED
#define DISTRHO_PLUGIN_INFO_H_INCLUDED

#define DISTRHO_PLUGIN_NAME  "Clock"
#define DISTRHO_PLUGIN_URI   "http://ullo.fr/botania/clock"
#define DISTRHO_PLUGIN_CLAP_ID "botania.clock"

#define DISTRHO_PLUGIN_NUM_INPUTS   0
// Note: all CV outputs
#define DISTRHO_PLUGIN_NUM_OUTPUTS  5
#define DISTRHO_PLUGIN_WANT_MIDI_INPUT 0
#define DISTRHO_PLUGIN_WANT_MIDI_OUTPUT 0
#define DISTRHO_PLUGIN_IS_RT_SAFE   1
#define DISTRHO_PLUGIN_WANT_TIMEPOS 1

enum Parameters {
    kBPM,
    kSwing,
    kOrderMix,
    kGroupSize,
    kGroupRatio,
    kTicks,
    
    kParameterCount
};

#endif
