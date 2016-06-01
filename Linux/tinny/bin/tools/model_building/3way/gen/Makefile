############################################################################
#
# Usage: make -j n -l load [NJOBS=num]
#
#		n: max number of nnet run simutaniousely
#	load: max load
#   num: max scale and eval run simutaniousely, default 10
#
############################################################################

SHELL = /bin/sh
CWD = $(shell pwd)

# Get global settings
include Config.mk

ifeq ($(NJOBS), )
	NJOBS = 10
endif

## Targets
SCALE = scale/.done
MODEL = $(foreach a, ${MODEL_IX}, model/.done-model-${a})
SENS = $(foreach a, ${MODEL_IX}, model/.done-sens-${a})
EVAL = $(foreach a, ${MODEL_IX}, eval/.done-res-${a})

ALL = ${EVAL}
ifeq ($(SENSANA), YES)
	ALL += ${SENS}
endif

## Rules

all : ${ALL}

scale : ${SCALE}
model : ${MODEL}
sens : ${SENS}
eval : ${EVAL}

${SCALE} : 
	+ cd scale;\
	make -j${NJOBS};\
	touch .done
	
model/.done-model-% : ${SCALE}
	+ cd model;\
	make MODEL_INC=Model-$*.mk > $*-model.log 2>&1;\
	touch .done-model-$*

model/.done-sens-% : model/.done-model-%
	+ cd model;\
	make sensitivity MODEL_INC=Model-$*.mk > $*-sens.log 2>&1;\
	touch .done-sens-$*
		
eval/.done-res-% : model/.done-model-%
	+ cd eval;\
	make -j${NJOBS} MODEL_INC=Model-$*.mk > $*.log 2>&1;\
	touch .done-res-$*

	
	
clean: clean-scale clean-model clean-eval

clean-scale :
	cd scale;\
	make clean
clean-model :
	cd model;\
	make clean
clean-eval :
	cd eval;\
	make clean
