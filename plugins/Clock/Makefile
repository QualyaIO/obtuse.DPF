#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# --------------------------------------------------------------
# Project name, used for binaries

NAME = clock

# --------------------------------------------------------------
# Files to build

FILES_DSP = \
	../../botania/src/vultin.cpp \
	../../botania/src/utils.cpp \
	Clock.cpp

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

ifneq ($(MACOS_OR_WINDOWS),true)
ifeq ($(HAVE_LIBLO),true)
TARGETS += dssi
endif # HAVE_LIBLO
endif # MACOS_OR_WINDOWS

TARGETS += lv2_sep
TARGETS += vst2
TARGETS += vst3
TARGETS += clap

endif # HAVE_OPENGL

all: $(TARGETS)
