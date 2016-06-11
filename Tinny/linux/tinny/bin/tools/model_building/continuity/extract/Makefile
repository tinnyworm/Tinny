SHELL = /bin/sh

## parameters
RANGE = 20030515 20030521
SCORES = 0 500 550 600 650 700 750 800 850 900 950
WEEKS = 0313 0320 0327 0403 0410 0417 0424 0501 0508 0515 0522

## targets
FRAUDOUT = $(foreach a, ${WEEKS}, fraud-${a}.out)
NONFRAUDOUT = $(foreach a, ${WEEKS}, nonfraud-${a}.out)

## excutables
GETFRAUD = ./getFraudCases.sh
GETNONFRAUD = ./getNonFraudCases.sh

## rules
all : fraud.out nonfraud.out

fraud-%.out : 
	${GETFRAUD} $* ${RANGE} > $@

nonfraud-%.out : 
	${GETNONFRAUD} $* ${RANGE} > $@

fraud.out : ${FRAUDOUT}
	cat $? > $@

nonfraud.out : ${NONFRAUDOUT}
	cat $? > $@
