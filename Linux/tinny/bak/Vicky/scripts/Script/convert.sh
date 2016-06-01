#! /bin/sh
#

zz=/usr/local/bin/zcat 

for file in `ls *gz`
do
key=`echo $file | sed "s:\.: :g" | awk '{print $2}'`
dir=`echo $file | sed "s:\.: :g" | awk '{print $1}'`
case "$key" in
CARD)      num=`echo "291"` ;;  
AUTH)      num=`echo "324"` ;;
POST)      num=`echo "318"` ;;
PAY)       num=`echo "115"` ;;
NONM)      num=`echo "63"`  ;;
FRAUD)     num=`echo "91"`  ;;
*)       echo "what is $file" ;;
esac
echo $file
$zz $file | dd conv=ascii | addnl $num | gzip > /ana/mds_files1/iFALCON/SBB/$dir/$file
sleep 10
echo " "
done
banner done
