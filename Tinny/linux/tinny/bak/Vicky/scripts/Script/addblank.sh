#!  /usr/local/bin/bash
#

zz=/usr/local/bin/zcat

for file in `ls *182*`
do
   nmfile=`echo $file | sed "s/182/195/"`
   $zz $file | ../padd.py 196 | gzip >/work/nobackup1_tmp1/jaccs/temp/$nmfile
done


