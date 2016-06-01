#! /bin/sh
#  read the dlt data


tape=/dev/rmt/2n

banner begin

mt -t $tape rew
sleep 2
mt -t $tape fsf 1
sleep 2

for file in `cat 900001254.filelist`
do

dd if=$tape bs=32768 of=1174/$file
echo "$? `date` $file"
sleep 2
echo " "

done

banner done
