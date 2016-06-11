############################################################
#	make -j4
#	Don't use more than 4 jobs on Lux-machines
#       Don't use more than 10 jobs on UX52 or Fraud12
#       Don't try parallel on snap machines
############################################################
SHELL = /usr/local/bin/bash
OS = $(shell uname)

############################################################
DELAY = 17
FIRST_WEEK = 745
		#> 2day90 20021001|awk '{print int(($6-17)/7)}'
LAST_WEEK = 826
#LAST_WEEK = 731
		#> 2day90 20031231|awk '{print int(($6-17)/7)}'
MP_DIR = MP_DIR
DAT = 00_dataIn
TOOLS = ./TOOLS/${OS}/

############ For MPB10 FP ##################################
# APP_PL = ${TOOLS}/mp_app
############################################################

############ For MPB20 FP ##################################
APP_PL = ${TOOLS}/mp_app -m2
############################################################

############ For MPB10 RF ##################################
# APP_PL = ${TOOLS}/mp_app -c mcc=187,mid=286,cnt=191 -l 1886 -t
# Qi, please check the "-l" setting.
############################################################

############ For MPB20 RF ##################################
# APP_PL = ${TOOLS}/mp_app -m2 -c mcc=187,mid=286,cnt=191 -l 1886 -t
############################################################

UFALBIN = /work/price/falcon/${OS}/bin

DIRS = 00_out_unsorted authsIn authsOut errsOut mergeSplit
WEEKS = $(shell for ((i=${FIRST_WEEK};i<=${LAST_WEEK};i++));do printf "%05i " $$i;done)
N8 = 0 1 2 3 4 5 6 7 
SORTED = $(foreach i, ${WEEKS}, auths.sorted.${i}.0.gz)
APPENDED = $(foreach i, ${WEEKS}, auths.appended.${i}.0.gz)
MERGE = $(foreach i, ${N8}, consortium_auths.mpb.$i.gz)

#rules
all: ${MERGE} ${DIRS}
	mv auths.appended.*.gz authsOut
	mv auths.sorted.*.gz authsIn
	mv auths.*.gz 00_out_unsorted
	mv consortium_auths.mpb* mergeSplit
	mv err.* errsOut
	cat errsOut/err.append.* | ${TOOLS}/final_check.pl

${DIRS}:
	mkdir $@

consortium_auths.mpb.%.gz: append
	for i in auths.appended.*.$*.gz; do \
		if [ `gzip -l $$i | awk '$$1!="compressed"{print $$2}'` = "0" ] ; then \
			rm -f $$i ;\
			echo "WARNING: $$i is empty, has been removed!" >> err.merg.$* ;\
		fi;\
	done
	${UFALBIN}/col_merge -c1-33 auths.appended.*.$*.gz 2>>err.merg.$* \
	| gzip > $@

append: ${APPENDED}
	touch append

auths.appended.%.0.gz:auths.sorted.%.gz
	gzip -dc auths.sorted.$*.gz| ${APP_PL} ${MP_DIR}/MPB-2.0-$**SUMMARY \
		2>err.append.$* \
		| ${UFALBIN}/bmsort -m1000 -S -c1-33 \
		| ${TOOLS}/split_by_hash auths.appended.$* 

auths.sorted.%.gz:split 
	gzip -dc auths.$*.gz \
	| ${UFALBIN}/bmsort -c85-88,124-139 -S -m1000 \
	| gzip -c > $@

split: 
	test -e ${DAT}/auths.dat.gz
	gzip -cd ${DAT}/auths.dat.gz \
	| ${UFALBIN}/printableOnly 2> err.pr.out \
	| ${TOOLS}/split_by_w90 -w${FIRST_WEEK}-${LAST_WEEK} \
		-s20 -d${DELAY} auths
	awk '/Line Length *Number *Pct/{flg=NR;}flg==NR-1&&NR>1{if( int($$3) != 100){ exit 1;} }' err.pr.out
	touch split
clean:
	rm -rf ${DIRS}  split append auths.*gz err.*
