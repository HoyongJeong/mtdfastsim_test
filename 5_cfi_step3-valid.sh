# Basen on runTheMatrix.py --what upgrade -l 36834.0 --dryRun
cmsDriver.py step3-valid                                        \
	-s HARVESTING:@trackingOnlyValidation                   \
	--no_exec                                               \
	--python_filename step3-valid_cfg.py                    \
	--conditions auto:phase2_realistic_T35                  \
	--mc                                                    \
#	--fast                                                  \
	--geometry ExtendedRun4D121                             \
	--scenario pp                                           \
	--filetype DQM                                          \
	--filein file:step2-fast_inDQM.root                     \
	--era Phase2C22I13M9_FastSim                            \
	-n 1                                                    \
	--fileout file:step3-valid.root > 5_cfi_step3-valid.log \ 
	2>&1
