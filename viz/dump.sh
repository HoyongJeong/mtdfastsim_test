#!/bin/bash

cmsRun dump_cfg.py inputFiles=file:../20_1_X/36889.0/step2-fast.root outputFile=hits_fast.root simLabel=fastSimProducer maxEvents=-1
cmsRun dump_cfg.py inputFiles=file:../20_1_X/36889.0/step2-full.root outputFile=hits_full.root simLabel=g4SimHits
