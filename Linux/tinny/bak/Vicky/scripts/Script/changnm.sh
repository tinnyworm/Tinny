#! /bin/sh
# change name as 20..

for file in `cat /work/gold/TAPELIST/extended/*1447.filelist`
do
  filename=`echo $file | /home/yxc/test/namepart.py`
  pre=`echo $filename | sed -e "s:\.: :" | awk '{print $1}'`
  post=`echo $filename | sed -e "s:\.: :" | awk '{print $2}'`
  echo $pre.20$post
   
done
   

