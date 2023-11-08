
#ifndef DISTRHO_PLUGIN_INFO_H_INCLUDED
#define DISTRHO_PLUGIN_INFO_H_INCLUDED

#define DISTRHO_PLUGIN_NAME  "SynthFMr"
#define DISTRHO_PLUGIN_URI   "https://gitlab.com/ullo/funky/botania"
#define DISTRHO_PLUGIN_CLAP_ID "botania.synthfm"

#define DISTRHO_PLUGIN_NUM_INPUTS   1
#define DISTRHO_PLUGIN_NUM_OUTPUTS  1
#define DISTRHO_PLUGIN_IS_RT_SAFE   1

enum Parameters {
    kModulatorAttack,
    kModulatorDecay,
    kModulatorSustain,
    kModulatorRelease,
    kModulatorRatio,
    kModulatorWavetable,
    kModulatorWavetablePhase,
    kModulatorLevel,
    kModulatorMode,
    kModulatorShift,
    kModulatorFeedback,

    kCarrierAttack,
    kCarrierDecay,
    kCarrierSustain,
    kCarrierRelease,
    kCarrierRatio,
    kCarrierWavetable,
    kCarrierWavetablePhase,

    kParameterCount
};

#endif
