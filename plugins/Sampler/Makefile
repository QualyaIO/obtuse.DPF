#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# --------------------------------------------------------------
# Project name, used for binaries

NAME = sampler

# --------------------------------------------------------------
# Files to build

FILES_DSP = \
	../../obtuse/src/vultin.cpp \
        ../../obtuse/src/synthSampler.cpp \
        ../../obtuse/src/synthSamplerCello.cpp \
        ../../obtuse/src/synthSamplerClarinet.cpp \
        ../../obtuse/src/synthSamplerPiano.cpp \
        ../../obtuse/src/synthSamplerAmbientLead.cpp \
        ../../obtuse/src/synthSamplerAuthenticStrings.cpp \
        ../../obtuse/src/synthSamplerBoesendorferGrandPiano.cpp \
        ../../obtuse/src/synthSamplerCelesta.cpp \
        ../../obtuse/src/synthSamplerSoftClarinet.cpp \
        ../../obtuse/src/synthSamplerClassicSuitcaseMk1.cpp \
        ../../obtuse/src/synthSamplerCosmos.cpp \
        ../../obtuse/src/synthSamplerDreamVoice.cpp \
        ../../obtuse/src/synthSamplerDrivenSynthLead.cpp \
        ../../obtuse/src/synthSamplerGlassMarimbaSoft.cpp \
        ../../obtuse/src/synthSamplerIndianSitar.cpp \
        ../../obtuse/src/synthSamplerJapaneseShakuhachiFlute.cpp \
        ../../obtuse/src/synthSamplerKoto.cpp \
        ../../obtuse/src/synthSamplerMagnificentDrone.cpp \
        ../../obtuse/src/synthSamplerMysticVibes.cpp \
        ../../obtuse/src/synthSamplerPowerFifths.cpp \
        ../../obtuse/src/synthSamplerPulsingPatterns.cpp \
        ../../obtuse/src/synthSamplerShootingStar.cpp \
        ../../obtuse/src/synthSamplerTimeWarp.cpp \
        ../../obtuse/src/synthSamplerTranquility.cpp \
        ../../obtuse/src/synthSamplerVocalCluster.cpp \
        ../../obtuse/src/synthSamplerZenGarden.cpp \
	MetaSampler.cpp \
	Sampler.cpp

# --------------------------------------------------------------
# Do some magic

include ../../dpf/Makefile.plugins.mk

# --------------------------------------------------------------
# Adding obtuse (ignoring its warnings) and custom libs
BUILD_CXX_FLAGS += -isystem../../obtuse/src -I../../src

# --------------------------------------------------------------
# Select plugins

TARGETS += lv2_sep
TARGETS += vst2
TARGETS += vst3
TARGETS += clap

all: $(TARGETS)
