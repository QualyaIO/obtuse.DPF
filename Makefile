#!/usr/bin/make -f
# Makefile for DPF #
# ---------------- #
# Created by falkTX
#

include dpf/Makefile.base.mk

BUILD_CXX_FLAGS = -Ibotania/src

all: dgl examples gen

# --------------------------------------------------------------

ifneq ($(CROSS_COMPILING),true)
CAN_GENERATE_TTL = true
else ifneq ($(EXE_WRAPPER),)
CAN_GENERATE_TTL = true
endif

dgl:
ifeq ($(HAVE_DGL),true)
	$(MAKE) -C dpf/dgl
endif

plugins: dgl
	$(MAKE) all -C plugins/Saturator

ifeq ($(CAN_GENERATE_TTL),true)
gen: plugins utils/lv2_ttl_generator
	@$(CURDIR)/dpf/utils/generate-ttl.sh

utils/lv2_ttl_generator:
	$(MAKE) -C dpf/utils/lv2-ttl-generator
else
gen:
endif

tests: dgl
	$(MAKE) -C tests

# --------------------------------------------------------------

clean:
	$(MAKE) clean -C dgl
	$(MAKE) clean -C examples/CVPort
	$(MAKE) clean -C examples/CairoUI
	$(MAKE) clean -C examples/EmbedExternalUI
	$(MAKE) clean -C examples/FileHandling
	$(MAKE) clean -C examples/Info
	$(MAKE) clean -C examples/Latency
	$(MAKE) clean -C examples/Meters
	$(MAKE) clean -C examples/Metronome
	$(MAKE) clean -C examples/MidiThrough
	$(MAKE) clean -C examples/Parameters
	$(MAKE) clean -C examples/SendNote
	$(MAKE) clean -C examples/States
	$(MAKE) clean -C utils/lv2-ttl-generator
	rm -rf bin build

# --------------------------------------------------------------

.PHONY: dgl examples tests
