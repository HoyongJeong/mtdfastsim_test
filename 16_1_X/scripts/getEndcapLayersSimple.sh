python3 -c "
import FWCore.ParameterSet.Config as cms
process = cms.Process('TEST')
process.load('FastSimulation.Configuration.SimIdeal_cff')

from FastSimulation.MTD.MTDFastSim_cff import customiseMTDFastSim
process = customiseMTDFastSim(process)

print('=== All EndcapLayers after customisation ===')
for i, layer in enumerate(process.fastSimProducer.trackerDefinition.EndcapLayers):
    if hasattr(layer, 'z'):
        print(f'layer {i}: z={layer.z.value()} cm, models={list(layer.interactionModels)}')
    else:
        print(f'layer {i}: no z attribute')
"
