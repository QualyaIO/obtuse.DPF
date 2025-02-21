
Audio plugins version of [Obtuse DSP](https://github.com/QualyaIO/obtuse.DSP) (VST2, VST3, LV2 and CLAP formats), thanks to [DPF](https://github.com/DISTRHO/DPF/). 

# Known issues

- VST3: some hosts do not support "internal" parameters and will display numerous extra parameters related to MIDI CC,see https://github.com/DISTRHO/DPF/issues/345
- VST3: some hosts (e.g. Ardour) will prevent automation when MIDI input is set.
- seeds for arp, chord and trigg can be the same for several plugins when they are instantiated at the same time

# TODO

- option to get and manually set seed where applicable?

# Dev

Using git submodule to get source from obtuse (itself using git LFS), and also from DPF. Don't forget to clone recursive or init submodule afterwards -- e.g. `git submodule update --init --recursive`.

# Changelog

## v0.1.0 (2025-02-21)

- first proper release
