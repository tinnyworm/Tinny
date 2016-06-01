#! /usr/local/bin/bash
#

zz=/usr/local/bin/zcat
zort=`echo "bmsort -T /tmp -S"`
ss=/work/price/falcon/dvl/ccpp/utils/sampstats/pcd_tmp/sampstats
tape=/dev/Ndlt.1
frauds=/work/gold/tape-processing/DAIEI/FRAUDS/frauds.clean.200202_200304.gz

mt -t $tape rewind
sleep 2
mt -t $tape fsf 1
sleep 2

for file in `cat /work/gold/TAPELIST/extended/900001346.filelist`
do
	filename=`echo $file | /home/yxc/test/namepart.py`
	qualifier=`echo $filename | sed "s:\.: :g" | gawk '{print $4}'`
	case "$qualifier" in
		00195)           key=`echo "-c1-33"`       ; flag=a ;;
		00202)           key=`echo "-c1-19,91-98"` ; flag=c ;;
		00179)           key=`echo "-c1-19,49-56"` ; flag=p ;;
	esac 
	dd if=$tape bs=32768 | $zz | $ss -s1 -f$frauds -$flag -ostats/$filename | $zort \
		$key | gzip > datas/$filename
	echo "$? `date` $filename"
done

sleep 2
mt -t $tape rewind
sleep 2
