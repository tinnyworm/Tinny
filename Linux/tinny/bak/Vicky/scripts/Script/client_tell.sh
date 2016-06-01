#! /bin/sh
#

zz=/usr/local/bin/zcat

for file in `ls */*gz`
do
filename=`echo $file | sed "s:\.: :g" | awk '{print $2}'`
echo $filename
$zz $file | wc -lc
sleep 2
/home/yxc/scripts/Python/client_tell.py hk.gz sub_HK/$file sub_CHINA/$file sub_THAI/$file sub_BRUNEI/$file sub_SRILAN/$file \
sub_INDON/$file sub_INDIA/$file sub_MACAU/$file sub_PHILI/$file sub_MALAY/$file sub_SINGA/$file sub_OTHER/$file
sleep 15
####  remove the empty file #######
ls -l sub*/$file | grep -v "  20 JUN" > temp
 for junk in `cat temp | awk '{print $8}'`
 do 
    ls -l $junk
    /bin/rm $junk
  done
  /bin/rm temp
  ls -l sub*/$file
  $zz sub*/$file | wc -lc
  sleep 2
done

banner done

