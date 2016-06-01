# This can only be run under SunOS currently, due to combineScaleFiles
# all data should be under ${DATA_DIR}, each sub directory for each data set
# with select file in it, the select file formated as
# select.mgen.${data_name}

include ../Config.mk

SHELL = /bin/sh

FALOPT  = -R silent
FALOPT += -z $(SCL_Z)
ifeq ($(FP), YES)
	FALOPT += -p mp=70
endif
ifeq ($(3WAY), YES)
	FALOPT += -t train # for 3way, always leave 25% for testing, 
	FALOPT += -b $*.bigScr.gz
else
	FALOPT += -t threshold=$(SCL_TEST_PCT)
	ifeq ($(SCL_GEN_BIGSCR), YES)
		FALOPT += -b $*.bigScr.gz
	endif
endif
ifneq ($(SCL_FILTER), )
	FALOPT += -f $(SCL_FILTER)
endif
ifneq ($(SCL_NUMBER), )
	FALOPT += -n $(SCL_NUMBER)
endif
ifneq ($(SCL_MOD), )
	FALOPT += -d $(SCL_MOD)
endif

## binaries
FALCONER = /work/gold/bin/falconer4
COMBSCL = /work/price/falcon/bin/combineScaleFiles
3WAYSPLIT = /home/dxl/tools/perl/3waysplit.pl
INPUTS = $(PROJECT).rul $(PROJECT).tmplt $(PROJECT).cnf $(BASENAME)\
	 vars\
	 tabs/$(NEW_RISK)\
	 nets/$(BASENAME)ScrCal

## targets
SUBBASE = $(foreach a, ${DATA}, ${a}_sub)
SUBSCL = $(foreach a, ${SUBBASE}, ${a}.scl)
SUBBIGSCR = $(foreach a, ${DATA}, ${a}.bigScr.gz)
## rules
all : ${PROJECT}.scl
prep: $(INPUTS)

${PROJECT}.scl : ${SUBSCL}
ifeq ($(3WAY), YES)
	${COMBSCL} --noKey -o ${PROJECT} ${SUBBASE};\
	zcat ${SUBBIGSCR} | gzip -c > ${PROJECT}.bigScr.gz;\
	${3WAYSPLIT} -t ${PROJECT} -b ${PROJECT}.bigScr.gz -p 19
else
	${COMBSCL} --noKey -o ${PROJECT} ${SUBBASE};
endif
	 
%_sub.scl : ${INPUTS}
	cp -p ${PROJECT}.cnf $*_sub.cnf;\
	${FALCONER} \
		-i dir=${DATA_DIR}/$*,select=${SELECT_DIR}/select.mgen.$* \
		${FALOPT} $*_sub.cnf > $*_sub.log 2>&1
	
$(PROJECT).rul: ../templates/basename.rul
	cat $<\
	 | sed "s/modmrule/$(BASENAME)/g"\
	 > $@
$(PROJECT).tmplt: ../templates/basename.tmplt
	cp $< $@
$(PROJECT).cnf: ../templates/basename.cnf
	cat $<\
	 | sed "s/modmrule/$(BASENAME)/g;\
		s/basename/$(PROJECT)/g"\
	 > $@
$(BASENAME): ../templates/modmrule
	cat $<\
	 | sed "s/risk_table_here/$(NEW_RISK)/g;\
		s/basename/$(BASENAME)/g;"\
	 > $@

tabs/$(NEW_RISK): tabs $(NEW_RISK_PATH)/$(NEW_RISK)
	cp $(NEW_RISK_PATH)/$(NEW_RISK) $@
tabs:
	mkdir $@;\
	cp ../templates/tabs/* $@
nets/$(BASENAME)ScrCal: nets
	cat ../templates/nets/basenameScrCal\
	| sed "s/BASENAME/$(BASENAME)/g"\
	> $@
nets:
	mkdir $@;
	cp ../templates/nets/varlists $@
vars:
	mkdir $@;\
	cp ../templates/vars/* $@

clean :
	find ./ ! -name Makefile -exec rm -r {} \;
