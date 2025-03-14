
#ifndef DISTRHO_PLUGIN_INFO_H_INCLUDED
#define DISTRHO_PLUGIN_INFO_H_INCLUDED

#define DISTRHO_PLUGIN_NAME  "ObtuseReverb"
#define DISTRHO_PLUGIN_URI   "https://qualya.io/obtuse/reverb"
#define DISTRHO_PLUGIN_CLAP_ID "obtuse.reverb"


#define DISTRHO_PLUGIN_NUM_INPUTS   1
#define DISTRHO_PLUGIN_NUM_OUTPUTS  1
#define DISTRHO_PLUGIN_IS_RT_SAFE   1

enum Parameters {
    kDryWet,
    kReverb,
    kDelay,
    kParameterCount
};

#endif
