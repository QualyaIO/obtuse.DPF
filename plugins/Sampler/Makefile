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
	../../botania/src/vultin.cpp \
	../../botania/src/synthFM.cpp \
        ../../botania/src/synthSampler.cpp \
        ../../botania/src/synthSamplerCello.cpp \
        ../../botania/src/synthSamplerClarinet.cpp \
        ../../botania/src/synthSamplerPiano.cpp \
        ../../botania/src/synthSamplerAmbientLead.cpp \
        ../../botania/src/synthSamplerAuthenticStrings.cpp \
        ../../botania/src/synthSamplerBoesendorferGrandPiano.cpp \
        ../../botania/src/synthSamplerCelesta.cpp \
        ../../botania/src/synthSamplerClarinets.cpp \
        ../../botania/src/synthSamplerClassicSuitcaseMk1.cpp \
        ../../botania/src/synthSamplerCosmos.cpp \
        ../../botania/src/synthSamplerDreamVoice.cpp \
        ../../botania/src/synthSamplerDrivenSynthLead.cpp \
        ../../botania/src/synthSamplerGlassMarimbaSoft.cpp \
        ../../botania/src/synthSamplerIndianSitar.cpp \
        ../../botania/src/synthSamplerJapaneseShakuhachiFlute.cpp \
        ../../botania/src/synthSamplerKoto.cpp \
        ../../botania/src/synthSamplerMagnificentDrone.cpp \
        ../../botania/src/synthSamplerMysticVibes.cpp \
        ../../botania/src/synthSamplerPowerFifths.cpp \
        ../../botania/src/synthSamplerPulsingPatterns.cpp \
        ../../botania/src/synthSamplerShootingStar.cpp \
        ../../botania/src/synthSamplerTimeWarp.cpp \
        ../../botania/src/synthSamplerTranquility.cpp \
        ../../botania/src/synthSamplerVocalCluster.cpp \
        ../../botania/src/synthSamplerZenGarden.cpp \
	MetaSampler.cpp \
	Sampler.cpp

# --------------------------------------------------------------
# Do some magic

include ../../dpf/Makefile.plugins.mk

# --------------------------------------------------------------
# Adding botania and custom libs
BUILD_CXX_FLAGS += -I../../botania/src -I../../src

# --------------------------------------------------------------
# Enable all possible plugin types

TARGETS += jack
TARGETS += dssi
TARGETS += lv2_sep
TARGETS += vst2
TARGETS += vst3
TARGETS += clap

all: $(TARGETS)
