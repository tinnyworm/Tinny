#! /bin/sh
# change name as 20..

for file in `ls */frauds__.0*00049*`
do
  dir=`echo $file | sed "s:\/: :" | gawk '{print $1}'`
  filename=`echo $file | /home/yxc/test/namepart.py`
  pre=`echo $filename | sed -e "s:\.: :" | awk '{print $1}'`
  post=`echo $filename | sed -e "s:\.: :" | awk '{print $2}'`
  nn=`echo $dir/$pre.20$post`
  #nn=`echo $dir/frauds__.$filename`
  echo $file
  echo $nn
  mv $file $nn
  sleep 1
  echo " "
   
done
   

