
Audio plugins version of Botania DSP, thanks to DPF. 

# Known issues

- VST3: some hosts do not support "internal" parameters and will display numerous extra parameters related to MIDI CC,see https://github.com/DISTRHO/DPF/issues/345
- VST3: some hosts (e.g. Ardour) will prevent automation when MIDI input is set.

# Dev

Using git submodule to get source from botania dsp (itself using git LFS), and also from DPF. Don't forget to clone recursive or init submodule afterwards -- e.g. `git submodule update --init --recursive`.
