#! /bin/sh
#  compute the size of each file

for file in `ls *credit*`
do
   #cat $file | wc -lc >>size
   echo $file
    temp=`echo $file | sed "s:\.: :" | gawk '{print $1}'`
    nn=`echo $temp.gz`
    cat $file | cut -c1-49 | gzip >$nn
    echo $nn
    echo " "
     /usr/local/bin/zcat $nn | wc -lc >>size

done      
