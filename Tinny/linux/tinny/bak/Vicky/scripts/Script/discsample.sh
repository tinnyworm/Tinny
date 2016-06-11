#! /usr/local/bin/bash
# finalized this sampling script

echo " Usage:     discsample.sh <frauds> <filelist> <filename_locate>"

zz="/usr/local/bin/zcat"
zort=`echo "bmsort -T /tmp -S"`
ss=/work/price/falcon/dvl/ccpp/utils/sampstats/pcd_tmp/sampstats
rmdr=/home/yxc/scripts/Perl/rmdir.pl
frauds=$1
filelist=$2
fkey=$3
sr=$4


for file in `cat $filelist `
do
		filename=`echo $file | $rmdr`
		num=`echo $filename | cut -d"." -f5`
		
        case "$fkey" in
          00164 | 00204 | 00324 | AUTH | AUTHS)     key=`echo "-c1-33"`       ; flag=a ;;
          00186 | 00291 | CARD  | CARDS)            key=`echo "-c1-19,91-98"` ; flag=c ;;
	  00107 | 00108 | 00115 | PAYS | PAY | PAYMS)     key=`echo "-c1-19,82-89"` ; flag=P ;;
	  00160 | 00201 | 00318 | POST | POSTS)     key=`echo "-c1-19,49-56"` ; flag=p ;;
          00047 | 00063 | NONM  | NONMS)            key=`echo "-c1-19,21-28"` ; flag=N ;;
		esac 

	
#		$zz $file  | dd conv=ascii | addnl $num | \
		$zz $file  | perl -ne 'chomp; print substr($_,3,16), "   ", substr($_,19,233), "\n"' | \
		$ss -s$sr -f$frauds -$flag -ostats/$filename | \
		$zort $key | gzip > datas/$filename
        echo $qualifier    $flag $num
        echo "$? `date` $filename"
	sleep 10 
done

banner done

echo "$filelist is done for sampling" | mail yaqingchen@fairisaac.com
