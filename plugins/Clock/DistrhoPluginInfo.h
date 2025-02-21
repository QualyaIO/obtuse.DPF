
#ifndef DISTRHO_PLUGIN_INFO_H_INCLUDED
#define DISTRHO_PLUGIN_INFO_H_INCLUDED

#define DISTRHO_PLUGIN_NAME  "ObtuseClock"
#define DISTRHO_PLUGIN_URI   "https://qualya.io/obtuse/clock"
#define DISTRHO_PLUGIN_CLAP_ID "obtuse.clock"

#define DISTRHO_PLUGIN_NUM_INPUTS   0
// Note: all CV outputs
#define DISTRHO_PLUGIN_NUM_OUTPUTS  5
#define DISTRHO_PLUGIN_WANT_MIDI_INPUT 0
#define DISTRHO_PLUGIN_WANT_MIDI_OUTPUT 0
#define DISTRHO_PLUGIN_IS_RT_SAFE   1
#define DISTRHO_PLUGIN_WANT_TIMEPOS 1

enum Parameters {
    kSource,
    kBPM,
    kSwing,
    kOrderMix,
    kGroupSize,
    kGroupRatio,
    kTicks,
    kNoteBarRatio,
    kOverrideOut1,
    kOverrideOut2,
    kOverrideOut3,
    kOverrideOut4,
    kOverrideOut5,

    kParameterCount
};

#endif
