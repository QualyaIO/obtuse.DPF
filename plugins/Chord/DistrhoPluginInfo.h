
#ifndef DISTRHO_PLUGIN_INFO_H_INCLUDED
#define DISTRHO_PLUGIN_INFO_H_INCLUDED

#define DISTRHO_PLUGIN_NAME  "ObtuseChord"
#define DISTRHO_PLUGIN_URI   "https://qualya.io/obtuse/chord"
#define DISTRHO_PLUGIN_CLAP_ID "obtuse.chord"

// Note: CV input
#define DISTRHO_PLUGIN_NUM_INPUTS   1
#define DISTRHO_PLUGIN_NUM_OUTPUTS  0
#define DISTRHO_PLUGIN_WANT_MIDI_INPUT 1
#define DISTRHO_PLUGIN_WANT_MIDI_OUTPUT 1
#define DISTRHO_PLUGIN_IS_RT_SAFE   1

enum Parameters {
    kChannelInput,
    kChannelTriggerInput,
    kScale,
    kChord,
    kChordSpread,
    kInvSpread,
    kChannelChord,
    kChannelScale,
    kJump,
    kRoot, // output

    kParameterCount
};

#endif
