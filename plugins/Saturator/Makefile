#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# --------------------------------------------------------------
# Project name, used for binaries

NAME = saturator

# --------------------------------------------------------------
# Files to build

FILES_DSP = \
	../../botania/src/effects.cpp \
	Saturator.cpp


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
