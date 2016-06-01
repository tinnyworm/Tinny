#############################################################################
# Makefile for build switch models for continuity test
# 
# Read the bigScr files from switch model for different settings (new_new and
# old_new). Extract new bigScr files and generate corresponding research
# files according the the $(RANGE)
#
# A perl utilities is required
# calc_date.pl: calculate date according the offset (under linux only)
#
# change DATE, RANGE, and KEY for different data and settings
#
# - dxl, 09/21/04
#############################################################################
SHELL = /bin/sh 

DATE = 20030522
RANGE = 0 1 2 3 4 5 6 7 8 9 10
KEY = __SWITCH_DATE__

## executables
CALCDATE = /home/dxl/tools/perl/calc_date.pl

## targets
CNFs = $(foreach a, ${RANGE}, switch-$(shell ${CALCDATE} ${DATE} -${a}weeks).cnf) 
RULs = $(foreach a, ${RANGE}, switch-$(shell ${CALCDATE} ${DATE} -${a}weeks).rul) 

## rules
all: ${CNFs} ${RULs}

switch-%.cnf : switch.cnf
	cat $< | sed "s/${KEY}/$*/" > $@

switch-%.rul : switch.rul
	cat $< | sed "s/${KEY}/$*/" > $@
