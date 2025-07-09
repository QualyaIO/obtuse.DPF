#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# --------------------------------------------------------------
# Project name, used for binaries

NAME = obtuse-reverb

# --------------------------------------------------------------
# Files to build

FILES_DSP = \
	../../obtuse/src/effects.cpp \
	../../obtuse/src/vultin.cpp \
	Reverb.cpp

FILES_UI = \
	ReverbUI.cpp 

# --------------------------------------------------------------
# Select plugins
# (to be set before including Makefile.rayui.mk for resources to work)
# note: for web export only jack will be forced

TARGETS += lv2_sep
TARGETS += vst2
TARGETS += vst3
TARGETS += clap
TARGETS += jack

# --------------------------------------------------------------
# Do some magic -- customized for using raylib UI and copying resources

include ../../dpf-extra/Makefile.rayui.mk

# --------------------------------------------------------------
# Adding obtuse (ignoring its warnings) and custom libs
BUILD_CXX_FLAGS += -isystem../../obtuse/src -I../../dpf-extra -I../../src 

# --------------------------------------------------------------
# And... action

all: raylib $(TARGETS) resources
