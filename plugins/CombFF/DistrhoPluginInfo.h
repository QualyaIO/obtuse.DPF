
#ifndef DISTRHO_PLUGIN_INFO_H_INCLUDED
#define DISTRHO_PLUGIN_INFO_H_INCLUDED

#define DISTRHO_PLUGIN_NAME  "CombFF"
#define DISTRHO_PLUGIN_URI   "https://qualya.io/obtuse/combff"
#define DISTRHO_PLUGIN_CLAP_ID "obtuse.combff"


#define DISTRHO_PLUGIN_NUM_INPUTS   1
#define DISTRHO_PLUGIN_NUM_OUTPUTS  1
#define DISTRHO_PLUGIN_IS_RT_SAFE   1

enum Parameters {
    kDryWet,
    kDecay,
    kDelay,
    kParameterCount
};

#endif
