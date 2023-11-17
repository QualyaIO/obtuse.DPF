#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# --------------------------------------------------------------
# Project name, used for binaries

NAME = arp

# --------------------------------------------------------------
# Files to build

FILES_DSP = \
	../../botania/src/vultin.cpp \
	../../botania/src/utils.cpp \
	Chord.cpp

# --------------------------------------------------------------
# Do some magic

include ../../dpf/Makefile.plugins.mk

# --------------------------------------------------------------
# Adding botania and custom libs
BUILD_CXX_FLAGS += -I../../botania/src -I../../src

# --------------------------------------------------------------
# Enable all possible plugin types

ifeq ($(HAVE_OPENGL),true)

TARGETS += jack

# Note: no dssi because we have MIDI out

TARGETS += lv2_sep
TARGETS += vst2
TARGETS += vst3
TARGETS += clap

endif # HAVE_OPENGL

all: $(TARGETS)
