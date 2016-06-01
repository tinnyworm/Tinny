#! /bin/sh


gtar=/usr/local/bin/gtar
for file in `cat filelist`
do
echo $file
inv=`echo $file | sed "s:\/: :g" | awk '{print $7}'`
echo $inv
cd $inv
$gtar xvf $file > utar.$inv.log
sleep 5
ls -l 
echo " "
cd ../
done

banner done
