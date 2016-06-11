#!/bin/sh
# change .gz into .dat.gz

for file in `cat /home/yxc/temp/900000724.filelist`
do 
   echo $file | sed "s/.gz/.dat.gz/g"

done
