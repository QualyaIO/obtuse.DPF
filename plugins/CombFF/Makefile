#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# --------------------------------------------------------------
# Project name, used for binaries

NAME = combFF

# --------------------------------------------------------------
# Files to build

# vultin.cpp here for win64 compilation during CI
FILES_DSP = \
	../../botania/src/vultin.cpp \
	../../botania/src/effectsXL.cpp \
	CombFF.cpp

# --------------------------------------------------------------
# Do some magic

include ../../dpf/Makefile.plugins.mk

# --------------------------------------------------------------
# Adding botania (ignoring its warnings) and custom libs
BUILD_CXX_FLAGS += -isystem../../botania/src -I../../src

# --------------------------------------------------------------
# Select plugins

TARGETS += lv2_sep
TARGETS += vst2
TARGETS += vst3
TARGETS += clap

all: $(TARGETS)
