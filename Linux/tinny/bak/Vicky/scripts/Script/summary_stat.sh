#! /bin/sh

file=$1
dir=$2


echo "$dir/$file:"
echo "Month Total-Record Totoal-Account Fraud-Rec Fraud-Acct NonFrd-Rec NonFrd-Acct"

for month in  0401 0402 0403 0404 0405 0406 0407 0408 0409 0410 0411 0412 0501 0502 0503 0504 0505 0506 0507 0508 0509 0510 0511

#for month in 0508 0509 0510 0511 0512 0601 0602
#for month in 0502 0503 0504
do 
total_rec=`cat $dir*/$month*$file*  | grep "Total number of records seen" | awk '{total += $7} END {print total}'` 
total_acct=`cat $dir*/$month*$file* | grep "Total number of accounts seen" | awk '{total += $7} END {print total}'`
frd_rec=`cat $dir*/$month*$file* | grep "Number of fraud records" | awk '{total += $6} END {print total}'`
frd_acct=`cat $dir*/$month*$file* | grep "Number of fraud accounts" | awk '{total += $6} END {print total}'`
non_frd_rec=`cat $dir*/$month*$file* | grep "non-fraud records in sample" | awk '{total += $8} END {print total}'`
non_frd_acct=`cat $dir*/$month*$file* | grep "non-fraud accounts in sample" | awk '{total += $7} END {print total}'`
#sample_rate=`cat $dir*/$month*$file* | grep "Actual record sampling rate" | | awk '{total += $7} END {print total}'`
echo "$month    $total_rec      $total_acct    $frd_rec    $frd_acct      $non_frd_rec     $non_frd_acct" 
done

echo " "
