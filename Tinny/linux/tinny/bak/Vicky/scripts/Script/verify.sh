#! /bin/sh
#

zz=/usr/local/bin/zcat
zort=`echo "bmsort -T /tmp -S"`

###############  Verify the auths month ################
echo "Verifying the auths files......."
for file in `ls datas/auth*`
do
echo $file 
$zz $file | cut -c20-25 | $zort | uniq -c | gawk '{print $2 "\t" $1}'
echo " "
sleep 2
done

sleep 1 

############## Verify the cards month #################
echo "Verifying the auths files......."
for file in `ls datas/card*`
do
echo $file 
$zz $file | cut -c91-98 | $zort | uniq -c | gawk '{print $2 "\t" $1}' 
echo " "
sleep 2
done
