# Based on runTheMatrix.py -w upgrade -l 29634.0 --dryRun --command="--no_exec"
cmsDriver.py TTbar_14TeV_TuneCP5_cfi \
	--no_exec \
	--python_filename full_cfg.py \
	-s             GEN,SIM \
	-n             10 \
	--conditions   auto:phase2_realistic_T33 \
	--beamspot     DBrealisticHLLHC \
	--datatier     GEN-SIM \
	--eventcontent FEVTDEBUG \
	--geometry     ExtendedRun4D110 \
	--era          Phase2C17I13M9 \
	--relval       9000,100 \
	--fileout file:step1.root \
	> step1_TTbar_14TeV+Run4D110.log \
	2>&1
