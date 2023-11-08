
#ifndef DISTRHO_PLUGIN_INFO_H_INCLUDED
#define DISTRHO_PLUGIN_INFO_H_INCLUDED

#define DISTRHO_PLUGIN_NAME  "Saturator"
#define DISTRHO_PLUGIN_URI   "http://ullo.fr/botania/saturator"
#define DISTRHO_PLUGIN_CLAP_ID "botania.saturator"


#define DISTRHO_PLUGIN_NUM_INPUTS   1
#define DISTRHO_PLUGIN_NUM_OUTPUTS  1
#define DISTRHO_PLUGIN_IS_RT_SAFE   1

enum Parameters {
    kThreshold,
    kCoeff,
    kParameterCount
};

#endif
