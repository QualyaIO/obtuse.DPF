#!umsr/bin/make -f
# Makefile for DPF #
# ---------------- #
# Created by falkTX
#

include dpf/Makefile.base.mk

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
	$(MAKE) all -C plugins/SynthFM
	$(MAKE) all -C plugins/Sampler
	$(MAKE) all -C plugins/Drummer
	$(MAKE) all -C plugins/Arp
	$(MAKE) all -C plugins/Chord
	$(MAKE) all -C plugins/Clock
	$(MAKE) all -C plugins/Gate
	$(MAKE) all -C plugins/Trigger
	$(MAKE) all -C plugins/SVF
	$(MAKE) all -C plugins/CombFF
	$(MAKE) all -C plugins/CombFB
	$(MAKE) all -C plugins/Allpass

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
	$(MAKE) clean -C plugins/Saturator
	$(MAKE) clean -C plugins/SynthFM
	$(MAKE) clean -C plugins/Sampler
	$(MAKE) clean -C plugins/Drummer
	$(MAKE) clean -C plugins/Arp
	$(MAKE) clean -C plugins/Chord
	$(MAKE) clean -C plugins/Clock
	$(MAKE) clean -C plugins/Gate
	$(MAKE) clean -C plugins/Trigger
	$(MAKE) clean -C plugins/SVF
	$(MAKE) clean -C plugins/CombFF
	$(MAKE) clean -C plugins/CombFB
	$(MAKE) clean -C plugins/Allpass
	$(MAKE) clean -C dpf/utils/lv2-ttl-generator
	rm -rf bin build

# --------------------------------------------------------------

.PHONY: dgl examples tests
