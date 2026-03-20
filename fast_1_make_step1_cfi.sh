# in: /afs/cern.ch/user/h/hojeong/fastsim/CMSSW_15_0_18/src going to execute cd 29634.0_TTbar_14TeV+Run4D110
cmsDriver.py TTbar_14TeV_TuneCP5_cfi \
	-s             GEN,SIM \
	--no_exec \
	--python_filename fast_cfg.py \
	-n             1 \
	--conditions   auto:phase2_realistic_T33 \
	--beamspot     DBrealisticHLLHC \
	--datatier     GEN-SIM \
	--eventcontent FEVTDEBUG \
	--geometry     ExtendedRun4D110 \
	--era          Phase2C17I13M9 \
	--relval       9000,100 \
	--fileout file:step1_fast.root \
	--customise FastSimulation/MTD/mtdSimHitProducer_cfi.customiseMTD \
	> fast_1_make_step1_cfi.log \
	2>&1
#	--fast \ Does not work with this matrix

# in: /afs/cern.ch/user/h/hojeong/fastsim/CMSSW_15_0_18/src going to execute cd 29634.0_TTbar_14TeV+Run4D110
#cmsDriver.py step2_fast \
#	-s             RECOBEFPIPE,RECO,HLT:@relvalRun4 \
#	--fast \
#	--conditions   auto:phase2_realistic_T33 \
#	--datatier     GEN-SIM-RECO \
#	-n             1 \
#	--eventcontent FEVTDEBUGHLT \
#	--geometry     ExtendedRun4D110 \
#	--era          Phase2C17I13M9 \
#	--filein       file:step1_fast.root \
#	--fileout      file:step2_fast.root \
#	> step2_fast.log \
#	2>&1
