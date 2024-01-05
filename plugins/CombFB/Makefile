#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# --------------------------------------------------------------
# Project name, used for binaries

NAME = combFB

# --------------------------------------------------------------
# Files to build

# vultin.cpp here for win64 compilation during CI
FILES_DSP = \
	../../botania/src/vultin.cpp \
	../../botania/src/effectsXL.cpp \
	CombFB.cpp

# --------------------------------------------------------------
# Do some magic

include ../../dpf/Makefile.plugins.mk

# --------------------------------------------------------------
# Adding botania (ignoring its warnings) and custom libs
BUILD_CXX_FLAGS += -isystem../../botania/src -I../../src

# --------------------------------------------------------------
# Enable all possible plugin types

TARGETS += jack
TARGETS += dssi
TARGETS += lv2_sep
TARGETS += vst2
TARGETS += vst3
TARGETS += clap

all: $(TARGETS)
