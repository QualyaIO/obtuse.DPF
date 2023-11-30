
#ifndef DISTRHO_PLUGIN_INFO_H_INCLUDED
#define DISTRHO_PLUGIN_INFO_H_INCLUDED

#define DISTRHO_PLUGIN_NAME  "Arp"
#define DISTRHO_PLUGIN_URI   "http://ullo.fr/botania/arp"
#define DISTRHO_PLUGIN_CLAP_ID "botania.arp"

// Note: CV input
#define DISTRHO_PLUGIN_NUM_INPUTS   1
#define DISTRHO_PLUGIN_NUM_OUTPUTS  0
#define DISTRHO_PLUGIN_WANT_MIDI_INPUT 1
#define DISTRHO_PLUGIN_WANT_MIDI_OUTPUT 1
#define DISTRHO_PLUGIN_IS_RT_SAFE   1

enum Parameters {
    kChannelInput, 
    kChannelTriggerInput, 
    kChannelOutput, 
    kMode,
    kRandNotes,
    kRandomize,
    kStepPersist,

    kParameterCount
};

#endif
