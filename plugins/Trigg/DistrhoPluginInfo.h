
#ifndef DISTRHO_PLUGIN_INFO_H_INCLUDED
#define DISTRHO_PLUGIN_INFO_H_INCLUDED

#define DISTRHO_PLUGIN_NAME  "ObtuseTrigg"
#define DISTRHO_PLUGIN_URI   "https://qualya.io/obtuse/trigg"
#define DISTRHO_PLUGIN_CLAP_ID "obtuse.trigg"

// Note: in and out are CV
#define DISTRHO_PLUGIN_NUM_INPUTS   1
#define DISTRHO_PLUGIN_NUM_OUTPUTS  1
#define DISTRHO_PLUGIN_WANT_MIDI_INPUT 0
#define DISTRHO_PLUGIN_WANT_MIDI_OUTPUT 0
#define DISTRHO_PLUGIN_IS_RT_SAFE   1
#define DISTRHO_PLUGIN_WANT_TIMEPOS 1

enum Parameters {
    kClockDivider,
    kClockShift,
    kLoopLength,
    kStartPos,
    kDensity,
    kBalance,
    kEvolve,
    kMagnitude,
    kDoTransportReset,

    kParameterCount
};

#endif
