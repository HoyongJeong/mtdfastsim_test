# in: /afs/cern.ch/user/h/hojeong/fastsim/CMSSW_15_0_18/src going to execute cd 29634.0_TTbar_14TeV+Run4D110
cmsDriver.py TTbar_14TeV_TuneCP5_cfi \
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

# in: /afs/cern.ch/user/h/hojeong/fastsim/CMSSW_15_0_18/src going to execute cd 29634.0_TTbar_14TeV+Run4D110
#cmsDriver.py step2 \
#	-s             DIGI:pdigi_valid,L1TrackTrigger,L1,L1P2GT,DIGI2RAW,HLT:@relvalRun4 \
#	--conditions   auto:phase2_realistic_T33 \
#	--datatier     GEN-SIM-DIGI-RAW \
#	-n             1 \
#	--eventcontent FEVTDEBUGHLT \
#	--geometry     ExtendedRun4D110 \
#	--era          Phase2C17I13M9 \
#	--filein       file:step1.root \
#	--fileout      file:step2.root \
#	> step2_TTbar_14TeV+Run4D110.log \
#	2>&1
