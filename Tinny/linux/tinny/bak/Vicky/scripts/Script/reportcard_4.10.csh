#!/bin/sh
# generate DQR

/work/gold/projects/bin/CCDS/3.10/AUTO_2.2/report_card2.2.csh \
-c /aisle09/client/HSBC_asia/sub_SRILAN/merge/cards.dat.gz \
-a /aisle09/client/HSBC_asia/sub_SRILAN/merge/auths.dat.gz \
-t /aisle09/client/HSBC_asia/sub_SRILAN/merge/posts.dat.gz \
-f /aisle09/client/HSBC_asia/sub_SRILAN/merge/frauds.dat.gz \
-n /aisle09/client/HSBC_asia/sub_SRILAN/merge/nonmons.dat.gz \
-p /aisle09/client/HSBC_asia/sub_SRILAN/merge/payments.dat.gz \
-i /aisle09/client/HSBC_asia/sub_SRILAN/merge/inqs.dat.gz \
-x /aisle09/client/HSBC_asia/sub_SRILAN/merge/intersect  \
-w /aisle01/ 

