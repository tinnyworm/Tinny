############################################################################
#
# Usage: gmake MODEL_INC=Model-08_0.mk
#
# Note: Eval_00.mk should always be in the root directory
#
############################################################################

#SHELL = /bin/sh 

# Get global settings
include ../Config.mk
## Get model-specific parameters
## The default model file 08_0 should always under root, otherwise 
## the command <make clean> wont's run becuase MODEL_INC is empty 
ifeq ($(MODEL_INC), )
  MODEL_INC = Model-08_0.mk
endif
include ../$(MODEL_INC)

# Get eval settings
include ../Eval_00.mk

#PYTHONPATH /work/gold/projects/lib
## Binaries
OS = $(shell uname)
PREP_MODEL = /work/gold/bin/prep_model
NN2WTA  = /home/dxl/tools/perl/nn2wta_dxl.pl
#NN2WTA = /work/price/falcon/bin/nn2wta
ifeq ($(OS), Linux)
	FALCONER = /work/gold/linux/bin/falconer4
	TAR = /bin/tar
	BIG2RES = /work/gold/linux/bin/big2res
else
	FALCONER = /work/gold/bin/falconer4
	TAR = /work/price/falcon/local/bin/tar
	BIG2RES = /work/gold/bin/big2res
endif

## Falconer options
FALOPT  = -R silent
#FALOPT += $(EVL_INPUT)
FALOPT += -t threshold=$(EVL_TEST_PCT),test
ifeq ($(FP), YES)
	FALOPT += -p mp=70
endif
ifeq ($(EVL_SCOREALL), YES)
	FALOPT += -S scoreall
endif
ifneq ($(EVL_FILTER), )
	FALOPT += -f $(EVL_FILTER)
endif
ifneq ($(EVL_NUMBER), )
	FALOPT += -n $(EVL_NUMBER)
endif
ifneq ($(EVL_MOD), )
	FALOPT += -d $(EVL_MOD)
endif
#ifeq ($(EVL_GEN_BIGSCR), YES)
#	FALOPT += -b $(BASENAME).bigScr.gz
#endif
ifeq ($(EVL_GEN_API), YES)
	FALOPT += -A $(BASENAME).API.gz
endif
ifeq ($(EVL_GEN_SCR), YES)
	FALOPT += -s $(BASENAME).shrtScr.gz
endif

EXP_NAME = $(BASENAME)-$(NN_NAME)-$(EVL_NAME)
EVAL_DIR = ${shell pwd}/eval-$(NN_NAME)-$(EVL_NAME)

####################################
# Targets
####################################

BIGSCR_OBJS = $(foreach a, ${DATA}, ${a}.bigScr.gz)

all: res.$(EXP_NAME)

####################################
# Rules
####################################

INPUTS = $(EVAL_DIR) nets tabs vars\
	 $(BASENAME).wta\
	 $(BASENAME).rrcd\
	 $(BASENAME)\
	 $(BASENAME).cfg\
	 $(PROJECT).cnf $(PROJECT).rul $(PROJECT).tmplt

$(BASENAME): ../scale/$(BASENAME)
	cp -f $< $(EVAL_DIR)/$@
$(BASENAME).cfg: ../model/$(BASENAME)-$(NN_NAME).CAP.cfg 
	cd $(EVAL_DIR); ln -s ../$< $@
$(PROJECT).cnf: ../scale/$(PROJECT).cnf
	cp -f $< $(EVAL_DIR)/$@
$(PROJECT).rul: ../scale/$(PROJECT).rul
	cp -f $< $(EVAL_DIR)/$@
$(PROJECT).tmplt: ../scale/$(PROJECT).tmplt
	cp -f $< $(EVAL_DIR)/$@

#$(EVAL_DIR):
#	mkdir $@; cd $@
$(EVAL_DIR):
	mkdir $@
nets: ../scale/nets
	cd $(EVAL_DIR); mkdir nets;\
	cd nets; ln -s ../../../scale/nets/* ../nets
tabs: ../scale/tabs
	cd $(EVAL_DIR); ln -s ../../scale/tabs .
vars: ../scale/vars
	cd $(EVAL_DIR); ln -s ../../scale/vars .

$(BASENAME).rrcd: ../model/$(BASENAME)-$(NN_NAME).rrcd 
	cd $(EVAL_DIR); ln -s ../$< $@
$(BASENAME).wta: ../model/$(BASENAME)-$(NN_NAME).nnw FORCE
	cd $(EVAL_DIR);\
	$(NN2WTA) ../$< $(basename $@)

.prep: $(INPUTS)
	cd $(EVAL_DIR);\
	$(PREP_MODEL)\
	 --score\
	 --reason\
	 $(BASENAME);\
	touch $@


%.bigScr.gz : .prep
	cd ${EVAL_DIR};\
	${FALCONER} \
		-i dir=${DATA_DIR}/$*,select=${SELECT_DIR}/select.muse.$* \
		${FALOPT} \
		-b ${EVAL_DIR}/$@ \
		-r file=${EVAL_DIR}/res.$*,full \
		${PROJECT}.cnf > ${EVAL_DIR}/$*.log 2>&1; 

res.$(EXP_NAME): ${BIGSCR_OBJS}
	cd $(EVAL_DIR);\
	zcat $? | ${BIG2RES} -s ${RATE} - $@
	
clean:
	find . -name CVS -prune -o \! -name Makefile \! -type d\
	  -exec rm -f \{\} \;
	find . -type d \! -name . \! -name CVS -exec \rm -r \{\} \;

FORCE:
