
#ifndef DISTRHO_PLUGIN_INFO_H_INCLUDED
#define DISTRHO_PLUGIN_INFO_H_INCLUDED

#define DISTRHO_PLUGIN_NAME  "ObtuseAllpass"
#define DISTRHO_PLUGIN_URI   "https://qualya.io/obtuse/allpass"
#define DISTRHO_PLUGIN_CLAP_ID "obtuse.allpass"


#define DISTRHO_PLUGIN_NUM_INPUTS   1
#define DISTRHO_PLUGIN_NUM_OUTPUTS  1
#define DISTRHO_PLUGIN_IS_RT_SAFE   1

#define DISTRHO_PLUGIN_HAS_UI 1
#define DISTRHO_UI_DEFAULT_WIDTH 396
#define DISTRHO_UI_DEFAULT_HEIGHT 346
#define DISTRHO_UI_USER_RESIZABLE 1
#define DISTRHO_UI_URI DISTRHO_PLUGIN_URI "#UI"

enum Parameters {
    kDryWet,
    kDecay,
    kDelay,
    kMaxDelay,
    kActivity,
    kParameterCount
};

#endif
