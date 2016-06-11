#! /bin/sh
# cut 4.10 to fit 3.10

zz=/usr/local/bin/zcat

for file in `ls *.00318*`
do
 nn=`echo $file | sed "s:\.00318.dat.gz:\.00179.dat.gz:"`
 echo $file
 echo $nn
 $zz $file | cut -c1-179 | gzip > $nn
 sleep 2
 echo " " 
 done
 
# for file in `ls *.00202*`
#do
# nn=`echo $file | sed "s:\.00202.dat.gz:\.00186.dat.gz:"`
# echo $file
# echo $nn
# $zz $file | cut -c1-186 | gzip > $nn
# sleep 2
# echo " " 
# done
