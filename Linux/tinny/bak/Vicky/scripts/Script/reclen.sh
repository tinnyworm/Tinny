#! /bin/sh 
#

zz=/usr/local/bin/zcat 

for file in `ls *post*`
do
 temp=`echo $file | sed "s:\.: :g" | gawk '{print $3 $5}'`
 $zz $file | sas reclen.sas > $temp
done
