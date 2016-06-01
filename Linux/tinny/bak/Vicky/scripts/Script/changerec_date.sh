#! /bin/sh
#

zz=/usr/local/bin/zcat

for file in `ls *CARD*`
do

month=`echo $file | sed "s:\.: :" | awk '{print $1}'`
case "$month" in
   0302)   echo $file; 
           $zz $file | awk '{print substr($0,1,90)"20030228"substr($0,99,186)}' | gzip >new_cards/$file ;;
   0303)   echo $file; 
           $zz $file | awk '{print substr($0,1,90)"20030331"substr($0,99,186)}' | gzip >new_cards/$file ;;
   0304)   echo $file; 
           $zz $file | awk '{print substr($0,1,90)"20030430"substr($0,99,186)}' | gzip >new_cards/$file ;;
   0305)   echo $file; 
           $zz $file | awk '{print substr($0,1,90)"20030531"substr($0,99,186)}' | gzip >new_cards/$file ;;
   0306)   echo $file; 
           $zz $file | awk '{print substr($0,1,90)"20030630"substr($0,99,186)}' | gzip >new_cards/$file ;;
   0307)   echo $file; 
           $zz $file | awk '{print substr($0,1,90)"20030731"substr($0,99,186)}' | gzip >new_cards/$file ;;
esac

  echo " "
  sleep 5
done  	   
   

for file in `ls new_cards/*`
do
echo $file
$zz $file | cut -c91-98 | sort | uniq -c | awk '{print $2 " " $1}'
echo " "
sleep 2
done

banner done	
