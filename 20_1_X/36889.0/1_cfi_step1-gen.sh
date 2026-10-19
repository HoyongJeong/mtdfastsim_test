#!/bin/bash

cmsDriver.py SingleMuPt15Eta1p7_2p7_cfi             \
	-s GEN                                          \
	--no_exec                                       \
	--python_filename step1-gen_cfg.py              \
	-n 10                                           \
	--conditions auto:phase2_realistic_T35_13TeV    \
	--beamspot Realistic25ns13TeVEarly2017Collision \
	--datatier GEN                                  \
	--eventcontent FEVTDEBUG                        \
	--geometry ExtendedRun4D121                     \
	--era Phase2C22I13M9_FastSim                    \
	--relval 9000,100                               \
	--fileout file:step1-gen.root                   \
	2>&1                                            \
	| tee 1_cfi_step1-gen.log                       \
