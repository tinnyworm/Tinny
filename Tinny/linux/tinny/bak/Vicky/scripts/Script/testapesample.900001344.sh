#! /usr/local/bin/bash
#

zz=/usr/local/bin/zcat
zort=`echo "bmsort -T /tmp -S"`
ss=/work/price/falcon/dvl/ccpp/utils/sampstats/pcd_tmp/sampstats
tape=/dev/Ndlt.2
frauds=/work/gold/tape-processing/CAP.ONE/FRAUDS/2002/frauds.clean.200201_200303.gz

mt -t $tape rewind
sleep 2
mt -t $tape fsf 2
sleep 2

for file in `cat /work/gold/TAPELIST/extended/900001344.filelist | grep -v "junk.txt"`
do
	filename=`echo $file | /home/yxc/test/namepart.py`
	qualifier=`echo $filename | sed "s:\.: :" | awk '{print $1}'`
	case "$qualifier" in
		auth)           key=`echo "-c1-33"`       ; flag=a ;;
		cardholder)     key=`echo "-c1-19,91-98"` ; flag=c ;;
		dompostedtrans) key=`echo "-c1-19,49-56"` ; flag=p ;;
		payment)        key=`echo "-c1-19,82-89"` ; flag=P ;;
	esac 
	dd if=$tape bs=32768 | $zz | $ss -s5 -f$frauds \
		-$flag -ousa/stats/$filename | $zort \
		$key | gzip > usa/datas/$filename
	echo "$? `date` $filename"
done

sleep 2
mt -t $tape rewind
sleep 2
