#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# --------------------------------------------------------------
# Project name, used for binaries

NAME = obtuse-drummer

# --------------------------------------------------------------
# Files to build

FILES_DSP = \
	../../obtuse/src/vultin.cpp \
        ../../obtuse/src/synthDrummer.cpp \
        ../../obtuse/src/synthDrummerBbox.cpp \
        ../../obtuse/src/synthDrummerFoleyType.cpp \
        ../../obtuse/src/synthDrummerNes.cpp \
        ../../obtuse/src/synthDrummerTamaRockstar.cpp \
	MetaDrummer.cpp \
	Drummer.cpp

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
