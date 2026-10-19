#!/bin/bash

cmsRun step1-gen_cfg.py       \
	2>&1                      \
	| tee 2_run_step1-gen.log \
