python3 -c "
import FWCore.ParameterSet.Config as cms
process = cms.Process('TEST')
process.load('FastSimulation.Configuration.SimIdeal_cff')

models = process.fastSimProducer.interactionModels
for name in models.parameterNames_():
    print(f'=== {name} ===')
    print(getattr(models, name))
    print()
"
