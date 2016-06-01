#!/bin/sh
# generate DQR

/work/gold/projects/bin/CCDS/3.10/AUTO_2.2/report_card2.2.csh \
-c ../merge/cards.dat.gz \
-a ../merge/auths.dat.gz \
-t ../merge/posts.dat.gz \
-n ../merge/nonmons.dat.gz \
-p ../merge/payments.dat.gz \
-f ../merge/frauds.dat.gz \
-x ../merge/intersect  \
-w /aisle01/ 

