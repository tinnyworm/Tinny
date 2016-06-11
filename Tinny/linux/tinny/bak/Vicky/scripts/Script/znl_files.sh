#! /bin/sh

znl=/work/price/falcon/bin//znl
#type=$1
monthlist=$1
dir=$2

for month in `cat $monthlist`
do
file=`echo $dir/*$month/$month*gz`
echo $month
$znl -c -v -f $file 
sleep 2
echo " "
done

banner done

