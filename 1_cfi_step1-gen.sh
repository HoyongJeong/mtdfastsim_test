# Basen on runTheMatrix.py --what upgrade -l 36834.0 --dryRun
cmsDriver.py TTbar_14TeV_TuneCP5_cfi                    \
	-s GEN                                          \
	--no_exec                                       \
	--python_filename step1-gen_cfg.py              \
	-n 1                                            \
	--conditions auto:phase2_realistic_T35          \
	--beamspot Realistic25ns13TeVEarly2017Collision \
	--datatier GEN                                  \
	--eventcontent FEVTDEBUG                        \
	--geometry ExtendedRun4D121                     \
	--era Phase2C22I13M9_FastSim                    \
	--relval 9000,100                               \
	--fileout file:step1-gen.root                   \
	> 1_cfi_step1-gen.log                           \
	2>&1
