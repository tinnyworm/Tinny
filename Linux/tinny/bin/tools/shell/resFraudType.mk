# Makefile to generate research file for different type
#
# Fraud Types
#
# 	ID: ID theft (A -- application fraud or T -- takeover)
# 	 C: Counterfeit (C -- counterfeit or G -- skimming)
# 	LS: Lost/stolen (L -- lost or S -- stolen)
# 	 N: Non-receipt (N -- non-receipt)
# 	MI: Moto and internet (M -- moto or I -- internet)
#
# Modify "BIGSCR" and "RATE" before use
#
# dxl: 09/2004


SHELL = /bin/sh

# sources (modify here if necessary)
BIGSCR = rbs_?.bigScr.gz
RATE = 0.03
PREFIX = res.ffm65.rbs.fraud

# targets
TARGETS = ${PREFIX}.ID \
          ${PREFIX}.C \
			 ${PREFIX}.LS \
			 ${PREFIX}.N \
			 ${PREFIX}.MI

# rules
all : ${TARGETS}

${PREFIX}.ID : ${BIGSCR}
	zcat $? | \
	perl -ne '$$f=substr($$_,79,1);if(($$f eq "A")||($$f eq "T")){print $$_;}'\
	| big2res -s ${RATE} - $@ 

${PREFIX}.C : ${BIGSCR}
	zcat $? | \
	perl -ne '$$f=substr($$_,79,1);if(($$f eq "C")||($$f eq "G")){print $$_;}'\
	| big2res -s ${RATE} - $@ 

${PREFIX}.LS : ${BIGSCR}
	zcat $? | \
	perl -ne '$$f=substr($$_,79,1);if(($$f eq "L")||($$f eq "S")){print $$_;}'\
	| big2res -s ${RATE} - $@ 

${PREFIX}.N : ${BIGSCR}
	zcat $? | \
	perl -ne '$$f=substr($$_,79,1);if($$f eq "N"){print $$_;}' \
	| big2res -s ${RATE} - $@ 

${PREFIX}.MI : ${BIGSCR}
	zcat $? | \
	perl -ne '$$f=substr($$_,79,1);if(($$f eq "M")||($$f eq "I")){print $$_;}'\
	| big2res -s ${RATE} - $@ 

