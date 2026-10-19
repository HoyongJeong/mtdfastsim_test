#!/bin/bash

cmsDriver.py step2-fast                    \
	-s SIM                                 \
	--no_exec                              \
	--python_filename step2-fast_cfg.py    \
	--fast                                 \
	--beamspot None                        \
	--conditions auto:phase2_realistic_T35 \
	--geometry ExtendedRun4D121            \
	--eventcontent FEVTDEBUG               \
	--datatier GEN-SIM                     \
	--era Phase2C22I13M9_FastSim           \
	-n 10                                  \
	--filein  file:step1-gen.root          \
	--fileout file:step2-fast.root         \
	2>&1                                   \
	| tee 3_cfi_step2-fast.log             \

