#! /usr/local/bin/bash
#
zz=/usr/local/bin/zcat

for file in `ls card*.dat.gz`
do 
$zz $file | wc -lc >> cardsize.txt
done

