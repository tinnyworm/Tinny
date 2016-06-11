#############################
## Settings for NNET training
#############################

# NNET binary
OS = $(shell uname)
ifeq ($(OS), Linux)
	NN_BIN = /work/price/falcon/$(OS)/bin/nnet-2.3.1
else
	NN_BIN = /work/price/falcon/bin/nnet-2.3.1
endif
# Number of hidden nodes
NN_HIDDEN = 8
# Number of training epochs
NN_EPOCHS = 1200
# Learning rate
NN_LRATE = 0.01
# Learning acceleration rate
NN_LRACCE = 1.02
# Learning deceleration rate
NN_LRDECE = 0.96
# Interlayer gain
NN_GAIN = 4.0
# Step size (semi-batching)
NN_STEP = 10
# Momentum term
NN_MOMENTUM = .4
# Random seed
NN_SEED = 119
# Allowable number of RMS increase epochs
NN_INCREASE = 100
# Use Costs [YES/NO] (Will probably need bigScrFile) Make sure to edit
#	Costs.mk in the model directory to specify how to calculate cost
#	function
NN_COSTS = NO
# Name of makefile for generating costs
COSTS_INC =
# Model Name - choose a name for this NNET configuration
NN_NAME = 08_0
