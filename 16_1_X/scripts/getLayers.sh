python3 -c "
import FWCore.ParameterSet.Config as cms
process = cms.Process('TEST')
process.load('FastSimulation.Configuration.SimIdeal_cff')

from FastSimulation.MTD.MTDFastSim_cff import customiseMTDFastSim
process = customiseMTDFastSim(process)

print('=== Tracker BarrelLayers (after customisation) ===')
for i, layer in enumerate(process.fastSimProducer.trackerDefinition.BarrelLayers):
    if hasattr(layer, 'radius'):
        print(f'layer {i}: r={layer.radius.value()} cm, models={list(layer.interactionModels)}')

print()
print('=== Tracker EndcapLayers (after customisation) ===')
for i, layer in enumerate(process.fastSimProducer.trackerDefinition.EndcapLayers):
    if hasattr(layer, 'z'):
        print(f'layer {i}: z={layer.z.value()} cm, models={list(layer.interactionModels)}')

print()
print('=== interactionModels ===')
for name in process.fastSimProducer.interactionModels.parameterNames_():
    print(f'  {name}')
"
