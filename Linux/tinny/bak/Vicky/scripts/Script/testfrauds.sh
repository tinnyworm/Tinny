#! /bin/sh/
#  test frauds format

zz=/usr/local/bin/zcat	
sas=/export/home/Applications/sas//sas
for file in `ls *.gz *.eof *.hdr`
do
  typekey=`echo $file | sed "s:\.: :g" | gawk '{print $4}'`
  case "$typekey" in 
  00204)  nn=`echo "auths___.20$file"`; mv $file $nn;;
  00186)  nn=`echo "cards___.20$file"`; mv $file $nn;;
  00047)  nn=`echo "nonmons_.20$file"`; mv $file $nn;;
  00108)  nn=`echo "payments.20$file"`; mv $file $nn;;
  *)      ehco "what is it?"; exit 1;;
  esac 
done
