#!/bin/csh -f
##
####
####              AUTHS
####
echo "`pwd`"
foreach i (`cat toc.auths`)
echo $i
/usr/local/bin/zcat 00Q3/$i | /work/price/falcon/bin/sampstats -s5 -ffrauds.dat -oSTATS/stats.$i -a | bmsort -S -c1-33 -m100 -t/work/bronze -T/work/bronze | gzip > SORTED/$i.sorted.dat.gz
date
end
####
####              CARDS
####
echo "`pwd`"
foreach i (`cat toc.cards`)
echo $i
/usr/local/bin/zcat 00Q3/$i | /work/price/falcon/bin/sampstats -s5 -ffrauds.dat -oSTATS/stats.$i -c | bmsort -S -c1-19,91-98 -m100 -t/work/bronze -T/work/bronze | gzip > SORTED/$i.sorted.dat.gz
date
end
####
####              POSTS
####
echo "`pwd`"
foreach i (`cat toc.posts`)
echo $i
/usr/local/bin/zcat 00Q3/$i | /work/price/falcon/bin/sampstats -s5 -ffrauds.dat -oSTATS/stats.$i -p | bmsort -S -c1-19,49-56 -m100 -t/work/bronze -T/work/bronze | gzip > SORTED/$i.sorted.dat.gz
date
end
#




unlimit
col_merge -c1-33 SORTED/*00182*gz | gzip > auths.dat.gz
#
#
unlimit
col_merge -c1-19,91-98 SORTED/*00202*gz | uniq | gzip > cards.dat.gz
#
#
unlimit
col_merge -c1-19,49-56 SORTED/*00179*gz | uniq | gzip > posts.dat.gz
#
