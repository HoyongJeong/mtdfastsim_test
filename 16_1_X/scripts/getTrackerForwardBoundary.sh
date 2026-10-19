python3 -c "
import FWCore.ParameterSet.Config as cms
process = cms.Process('TEST')
process.load('FastSimulation.Configuration.SimIdeal_cff')
print(process.fastSimProducer.trackerDefinition.trackerForwardBoundary)
"
