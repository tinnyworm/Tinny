#! /bin/sh

filelist=$3
outdir=$1
type=`echo $filelist | cut -d"." -f1`
client=$2

zz=/usr/local/bin/zcat 

for file in `cat $filelist`
do
month=`echo $file | cut -d"/" -f1`
inv=`echo $file | cut -d"_" -f2 | cut -c1-7`
 case "$inv" in 
  000000Z)    inv=`echo $file | cut -d"/" -f2 | cut -c1-6` ;;
   *)         inv=`echo $inv`;;
 esac
     case "$type" in 
        CARD)      num=`echo "291"`; key=`echo "-c91-98"`;
                   filename=`echo "$month.$client.$type.00$num.$inv.gz"` ;;
        AUTH)      num=`echo "324"`; key=`echo "-c20-25"`;
                   filename=`echo "$month.$client.$type.00$num.$inv.gz"` ;;
        NONM)      num=`echo "63"`;  key=`echo "-c21-26"`;
                   filename=`echo "$month.$client.$type.000$num.$inv.gz"` ;;
        FRAUD)     num=`echo "91"`;  key=`echo "-c36-41"`;                   
                   filename=`echo "$month.$client.$type.000$num.$inv.gz"` ;;
        PAYS)      num=`echo "115"`; key=`echo "-c82-87"`;
                   filename=`echo "$month.$client.$type.00$num.$inv.gz"` ;;
        POST)      num=`echo "318"`; key=`echo "-c49-54"`;
                   filename=`echo "$month.$client.$type.00$num.$inv.gz"` ;;
           *)       echo "what is $file?";;
     esac
$zz $file | dd conv=ascii | addnl $num | gzip > $outdir/$month/$filename
sleep 10
$zz  $outdir/$month/$filename | ufalcut $key | sort | uniq -c
sleep 10
echo "$? `date` $file |  $num  | $outdir/$month/$filename"
done

banner done
