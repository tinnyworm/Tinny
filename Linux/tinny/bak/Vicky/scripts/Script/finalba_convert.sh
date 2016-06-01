#! /bin/sh 
#

zz=/usr/local/bin/zcat
sas8=/export/home/Applications/sas/sas
banner begin
zort=`echo "bmsort -T /tmp -S"`
ss=/work/price/falcon/dvl/ccpp/utils/sampstats/pcd_tmp/sampstats
frauds=/work/gold/tape-processing/FINALBA/FRAUDS/frauds.clean.0107_0306.gz


for file in `ls 0303/*dec*gz 0304/*dec*gz 0305/*dec*gz 0306/*dec*gz`
do
dir=`echo $file | sed "s:\/: :g" | awk '{print $1}'`
filename=`echo $file | sed "s:\/: :g" | awk '{print $2}'`
key=`echo $filename | sed "s:\.: :g" | awk '{print $3}'`
case "$key" in 
 00211)   $zz $file | $sas8 /work/gold/tape-processing/FINALBA/ANALYSIS/ubs2ccds.c.sas | \
          $ss -s10 -f$frauds -c -o/aisle09/client/FINALBA/stats/$filename | $zort \
              -c1-19,91-98 | gzip >/aisle09/client/FINALBA/datas/$filename ; 
	  echo "$? `date` $filename";  sleep 20 ;;
 00221)   $zz $file | $sas8 /work/gold/tape-processing/FINALBA/ANALYSIS/ubs2ccds.a.sas | \
           $ss -s10 -f$frauds -c -o/aisle09/client/FINALBA/stats/$filename | $zort \
              -c1-33 | gzip >/aisle09/client/FINALBA/datas/$filename ; 
	  echo "$? `date` $filename";  sleep 20 ;;
	   
 00276)   $zz $file | $sas8 /work/gold/tape-processing/FINALBA/ANALYSIS/ubs2ccds.p.sas | \
          $ss -s10 -f$frauds -c -o/aisle09/client/FINALBA/stats/$filename | $zort \
              -c1-19,49-56 | gzip >/aisle09/client/FINALBA/datas/$filename ; 
	  echo "$? `date` $filename";  sleep 20 ;;
	   
   *)     echo $file;;
esac
sleep 2
echo " "
done
    	
sleep 2
banner done	
	  
