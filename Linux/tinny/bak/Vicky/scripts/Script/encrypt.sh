#! /bin/sh
#

zz=/usr/local/bin/zcat
encry=/home/yxc/test/encry
echo "Encrypting..."
for file in `ls *gz`
do
$zz $file | $encry | gzip > encrypt/$file
echo "$? `date` $file"
echo " "
sleep 10

done

banner done
