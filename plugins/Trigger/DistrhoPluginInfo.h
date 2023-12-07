
#ifndef DISTRHO_PLUGIN_INFO_H_INCLUDED
#define DISTRHO_PLUGIN_INFO_H_INCLUDED

#define DISTRHO_PLUGIN_NAME  "Trigger"
#define DISTRHO_PLUGIN_URI   "http://ullo.fr/botania/trigger"
#define DISTRHO_PLUGIN_CLAP_ID "botania.trigger"

// Note: in and out are CV
#define DISTRHO_PLUGIN_NUM_INPUTS   1
#define DISTRHO_PLUGIN_NUM_OUTPUTS  1
#define DISTRHO_PLUGIN_WANT_MIDI_INPUT 0
#define DISTRHO_PLUGIN_WANT_MIDI_OUTPUT 0
#define DISTRHO_PLUGIN_IS_RT_SAFE   1

enum Parameters {
    kClockDivider,
    kClockShift,
    kLoopLength,
    kStartPos,
    kDensity,
    kBalance,
    kEvolve,
    kMagnitude,

    kParameterCount
};

#endif
