import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

options = VarParsing('analysis')
options.register('simLabel', 'fastSimProducer', VarParsing.multiplicity.singleton, VarParsing.varType.string,
                 'module label of the SIM hits (g4SimHits or fastSimProducer)')
#options.register('genLabel', 'genParticles', VarParsing.multiplicity.singleton, VarParsing.varType.string,
#                 'label of the reco::GenParticle collection')
options.register('genLabel','generatorSmeared',VarParsing.multiplicity.singleton,VarParsing.varType.string,'...')
options.parseArguments()

process = cms.Process("DUMP")
process.load("FWCore.MessageService.MessageLogger_cfi")
process.load("Configuration.Geometry.GeometryExtendedRun4D121Reco_cff")	# D121 geometry (MTD digi geometry)
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(options.maxEvents))
process.source = cms.Source("PoolSource", fileNames = cms.untracked.vstring(options.inputFiles))
process.TFileService = cms.Service("TFileService", fileName = cms.string(options.outputFile))


process.dump = cms.EDAnalyzer("MTDHitDumper",
	gen    = cms.InputTag(options.genLabel),
	btl    = cms.InputTag(options.simLabel, "FastTimerHitsBarrel"),
	etl    = cms.InputTag(options.simLabel, "FastTimerHitsEndcap"),
	minPt  = cms.double(1.0),
	maxEta = cms.double(3.2),
)
process.p = cms.Path(process.dump)
