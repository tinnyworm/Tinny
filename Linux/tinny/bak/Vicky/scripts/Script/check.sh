#! /bin/sh
#

zz=/usr/local/bin/zcat

for file in `cat filelist`
do
echo $file
key=`echo $file | cut -c4-7`
case "$key" in
auth | AUTH)  col=`echo "-c20-25"` ;;
card | CARD)  col=`echo "-c91-98"` ;;
esac
$zz $file | cut $col | sort | uniq -c | awk '{print $2 " " $1}'
echo " "
done

banner done
