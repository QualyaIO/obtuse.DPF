
#ifndef PLUGIN_UTILS_H
#define PLUGIN_UTILS_H

// sync parameters' list the declared number of parameters
#include "DistrhoPluginInfo.h"

// sharing parameters info across DSP and UI.
const ParameterRanges params[kParameterCount] =
  {
    // default, min, max.
    ParameterRanges(0.5f, 0.0f, 1.0f), // dry/wet
    ParameterRanges(0.5f, 0.0f, 1.0f), // decay
   // actually max delay will depend on buffer size, with XL 16384 buffer and 44100 fs it's only 371ms
    ParameterRanges(50.0f, 0.0f, 1000.0f), // delay
  };
  
#endif /* PLUGIN_UTILS_H */
