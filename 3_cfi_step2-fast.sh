# Basen on runTheMatrix.py --what upgrade -l 36834.0 --dryRun
cmsDriver.py step2-fast                                                                                                 \
	-s SIM,RECOBEFMIX,DIGI:pdigi_valid,DIGI2RAW,RECO:reconstruction_trackingOnly,VALIDATION:@trackingOnlyValidation \
	--no_exec                                                                                                       \
	--python_filename step2-fast_cfg.py                                                                             \
	--fast                                                                                                          \
	--beamspot None                                                                                                 \
	--conditions auto:phase2_realistic_T35                                                                          \
	--geometry ExtendedRun4D121                                                                                     \
	--eventcontent RECOSIM,DQM                                                                                      \
	--datatier GEN-SIM-RECO,DQMIO                                                                                   \
	--era Phase2C22I13M9_FastSim                                                                                    \
	-n 1                                                                                                            \
	--filein  file:step1-gen.root                                                                                   \
	--fileout file:step2-fast.root > 3_cfi_step2-fast.log                                                           \
	2>&1
