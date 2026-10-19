python3 -c "
import FWCore.ParameterSet.Config as cms
process = cms.Process('TEST')
process.load('FastSimulation.Configuration.SimIdeal_cff')

from FastSimulation.MTD.MTDFastSim_cff import customiseMTDFastSim
process = customiseMTDFastSim(process)

print('=== BarrelLayers ===')
for i, layer in enumerate(process.fastSimProducer.trackerDefinition.BarrelLayers):
    if hasattr(layer, 'radius'):
        print(f'  [{i}] r={layer.radius.value()} models={list(layer.interactionModels)}')
    else:
        print(f'  [{i}] no radius - ', layer)
"
