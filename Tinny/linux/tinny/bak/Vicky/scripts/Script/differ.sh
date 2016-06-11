#! /bin/sh/
# compare two identical files for stats and datas

zz=/usr/local/bin/zcat

echo "uncompressing now...`date`"
for nob in `ls 02*/fr*`
do 
  nnob=`echo $nob| sed "s:\.dat.gz:\.dat:"`
  $zz $nob >$nnob
done

for god in `ls /work/gold/tape-processing/EQUIFAX-FALCON/FRAUDS/2002/*020*.gz`
do 
  ngod=`echo $god| sed "s:\.dat.gz:\.dat:"`
   $zz $god >$ngod
done

echo "uncompressing done... start comparing `date`"

for noback in `ls */*00049*.dat`
do
  nokey=`echo $noback | /home/yxc/test/namepart.py`
 
  for gold in `ls /work/gold/tape-processing/EQUIFAX-FALCON/FRAUDS/2002/*.dat`
    do 
   goldkey=`echo $gold | /home/yxc/test/namepart.py`
    case "$goldkey" in 
    $nokey)  diff $gold $noback; 
             echo "$noback";;
       *)    ;;
    esac
    done
done
 
 /bin/rm */*00049*.dat /work/gold/tape-processing/EQUIFAX-FALCON/FRAUDS/2002/*.dat   
 

