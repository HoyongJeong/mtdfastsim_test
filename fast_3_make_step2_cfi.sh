# Based on runTheMatrix.py -w upgrade -l 29634.0 --dryRun --command="--no_exec"
cmsDriver.py step2_fast \
	-s             RECOBEFPIPE,RECO,HLT:@relvalRun4 \
	--conditions   auto:phase2_realistic_T33 \
	--datatier     GEN-SIM-RECO \
	-n             1 \
	--eventcontent FEVTDEBUGHLT \
	--geometry     ExtendedRun4D110 \
	--era          Phase2C17I13M9 \
	--filein       file:step1_fast.root \
	--fileout      file:step2_fast.root \
	> step2_fast.log \
	2>&1
