
// tune wrapper class to fit vult fixed floats

#include "vultin.h"

// chunk size to process audio
// to sync with vult code
#define BUFFER_SIZE 128

// we hence use fixed float rather than float
#define BUFFER_TYPE fix16_t 

// function to convert back and forth with DPF buffers
#define BUFFER_IN_FUN(X) float_to_fix(X)
#define BUFFER_OUT_FUN(X) fix_to_float(X)

// to be incladed last to allow for override of macros
#include "ExtendedPlugin.hpp"
