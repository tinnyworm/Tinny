#! /usr/local/bin/bash
#
zz=/usr/local/bin/zcat
zort=`echo "bmsort -T /tmp -S"`
ss=/work/price/falcon/dvl/ccpp/utils/sampstats/pcd_tmp/sampstats
tape=/dev/Ndlt.2                
frauds=/work/gold/tape-processing/JACCS/FRAUDS/frauds.clean.200203_200303.gz

mt -t $tape rewind
sleep 2
mt -t $tape fsf 1
sleep 2

for file in `cat /work/gold/TAPELIST/extended/900001447.filelist` 
do
	filename=`echo $file | /home/yxc/test/namepart.py`  
	qualifier=`echo $filename | sed "s:\.: :" | awk '{print $1}'`
	case "$qualifier" in
		auth)           key=`echo "-c1-33"`       ; flag=a;;
		card)           key=`echo "-c1-19,91-98"` ; flag=c;;
		post)           key=`echo "-c1-19,49-56"` ; flag=p;;
		pay)            key=`echo "-c1-19,82-89"` ; flag=P;;
		*)                echo `what is this?`; exit 1;;
	esac 
	dd if=$tape bs=32768 | $zz | $ss -s1 -f$frauds -$flag -o/work/nobackup1_tmp1/JACCS/stats/$filename | $zort \
		$key | gzip > /work/nobackup1_tmp1/JACCS/datas/$filename
	echo "$? `date` $filename"

	sleep 10                                 
done
sleep 2
mt -t $tape rewind
sleep 2
