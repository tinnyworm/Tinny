#! /usr/local/bin/bash
#
zz=/usr/local/bin/zcat
zort=`echo "bmsort -T /tmp -S"`
ss=/work/price/falcon/bin//sampstats
tape=/dev/Ndlt.3                ######### NEED MODIFY THE TAPE NAME###
frauds=/work/gold/tape-processing/JACCS/FRAUDS/frauds.clean.200011_200112  ######### NEED FIND WHERE###

mt -t $tape rewind
sleep 2
mt -t $tape fsf 1
sleep 2

for file in `cat /home/yxc/jaccs/900000788.filelist`   ##### NEED MODIFY THE FILELIST NAME#####
do
	filename=`echo $file | /home/jcl/tools/namepart.py`  
	qualifier=`echo $filename | sed "s:\.: :" | awk '{print $1}'`
	case "$qualifier" in
		auth)           key=`echo "-c1-33"`       ; flag=a;;
		card)           key=`echo "-c1-19,91-98"` ; flag=c;;
		post)           key=`echo "-c1-19,49-56"` ; flag=p;;
		pay)            key=`echo "-c1-19,82-89"` ; flag=P;;
		*)                echo `what is this?`; exit 1;;
	esac 
	dd if=$tape bs=32768 | $zz | $ss -s1 -f$frauds -$flag -o/work/nobackup1_tmp1/jaccs/stats/$filename | $zort \
		$key | gzip > /work/nobackup1_tmp1/jaccs/datas/$filename
	echo "$? `date` $filename"

	sleep 10                                 
done
sleep 2
mt -t $tape rewind
sleep 2
