# Makefile for evaluation
#
# The last part of the select files should be the same as the 
# corresponding data directory. For example:
# 		DATA : DATA_DIR/cap1
#		SELECT : select.muse.cap1
#
# It can (and usually should) be run in parallel using
#		make -j<njobs> -l<maxload>
#			njobs: Allow N jobs at once; infinite jobs with no arg.
#			maxload: Don't start multiple jobs unless load is below N.
# See the help of make for detail information (using make --help)
# 
# It will always generate a big score file for each data set,
# <data>.bigScr.gz. The final research file will be named res.comb
# 
# dxl: 08/31/2004

SHELL = /bin/sh

DATA = veu_10 \
       veu_11 \
       veu_12 \
       veu_13 \
       veu_14 \
       veu_15 \
       veu_16 \
       veu_17 \
       veu_18 \
       veu_19

RATE = 0.01

MODEL_DIR = /work/price/falcon/models/fp/fp_veu_v2.0_2006/rel40
MODEL_NAME = fpveu20
DATA_DIR = /work/fpcurly/VISA_EU_2.0/data_final/MP2.0/veu_1
SEL_DIR = /work/fpcurly/VISA_EU_2.0/data_final/select
EVAL_DIR = ${shell pwd}

## Flags for Falconer4 options
# FP? [YES/NO]
FP = YES
# shrtScr? [YES/NO]
SCR = NO
# API? [YES/NO]
API = NO

## Falconer4 options
FALOPT = -R silent
ifeq (${FP}, YES)
	FALOPT += -p mp=70
endif
FALOPT += -t threshold=25,test
FALOPT += 
FALOPT += -d std_kchk
ifeq (${SCR}, YES)
	FALOPT += -s ${EVAL_DIR}/$*.shrtScr.gz
endif
ifeq (${API}, YES)
	FALOPT += -A ${EVAL_DIR}/$*.API.gz
endif

## Binaries
OS = $(shell uname)
ifeq (${OS}, SunOS)
	FALCONER = /work/gold/bin/falconer4
	BIG2RES = /work/gold/bin/big2res
else
	FALCONER = /work/gold/linux/bin/falconer4
	BIG2RES = /work/gold/linux/bin/big2res
endif

BIGSCR_OBJS = $(foreach a, ${DATA}, ${a}.bigScr.gz)

## rules
all : res.comb

res.comb : ${BIGSCR_OBJS}
	zcat $? | ${BIG2RES} -s ${RATE} - $@

%.bigScr.gz : 
	cd ${MODEL_DIR};\
	${FALCONER} \
		-i dir=${DATA_DIR}/$*,select=${SEL_DIR}/select.muse.$* \
		${FALOPT} \
		-b ${EVAL_DIR}/$@ \
		${MODEL_NAME}.cnf > ${EVAL_DIR}/$*.log 2>&1; 

.PHONY : clean_all clean_junk
clean_all : 
	find ./ ! -name Makefile -exec rm {} \;

clean_junk :
	-rm *.gz *.log *.out *~
