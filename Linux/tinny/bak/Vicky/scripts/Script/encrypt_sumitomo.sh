#! /usr/local/bin/bash

month=$1
encry=/home/yxc/scripts/C/encry

for file in `ls raw/$month/*FRAUD*gz`
do
echo $file
filename=`echo $file | /home/yxc/scripts/Perl/rmdir.pl`
/usr/local/bin/zcat $file | $encry | gzip -c > $month/$filename
sleep 2

done

ls -l $month/*gz

banner done
