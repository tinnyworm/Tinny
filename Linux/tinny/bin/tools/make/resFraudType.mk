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
BIGSCR = *.bigScr.gz
RATE = 0.03

# targets
TARGETS = res.fraud.ID \
          res.fraud.C \
			 res.fraud.LS \
			 res.fraud.N \
			 res.fraud.MI

# rules
all : ${TARGETS}

res.fraud.ID : ${BIGSCR}
	zcat $? | \
	perl -ne '$$f=substr($$_,79,1);if(($$f eq "A")||($$f eq "T")){print $$_;}'\
	| big2res -s ${RATE} - $@ 

res.fraud.C : ${BIGSCR}
	zcat $? | \
	perl -ne '$$f=substr($$_,79,1);if(($$f eq "C")||($$f eq "G")){print $$_;}'\
	| big2res -s ${RATE} - $@ 

res.fraud.LS : ${BIGSCR}
	zcat $? | \
	perl -ne '$$f=substr($$_,79,1);if(($$f eq "L")||($$f eq "S")){print $$_;}'\
	| big2res -s ${RATE} - $@ 

res.fraud.N : ${BIGSCR}
	zcat $? | \
	perl -ne '$$f=substr($$_,79,1);if($$f eq "N"){print $$_;}' \
	| big2res -s ${RATE} - $@ 

res.fraud.MI : ${BIGSCR}
	zcat $? | \
	perl -ne '$$f=substr($$_,79,1);if(($$f eq "M")||($$f eq "I")){print $$_;}'\
	| big2res -s ${RATE} - $@ 

