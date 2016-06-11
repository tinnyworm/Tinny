#! /bin/sh/
# change name

for file in `ls */200*`
do
dir=`echo $file | sed "s:\/: :" | gawk '{print $1}'`
filename=`echo $file | sed "s:\/: :" | gawk '{print $2}'`
key=`echo $filename| sed "s:\.: :g" | gawk '{print $4}'`
case "$key" in
00204) nn=`echo "auths___.$filename"`;;
00186) nn=`echo "cards___.$filename"`;; 
00108) nn=`echo "payments.$filename"`;;
00047) nn=`echo "nonmons_.$filename"`;;
00034) nn=`echo "aitf____.$filename"`;;
*)      echo "what is it? $file"; exit 1;;
esac
echo $file
echo $dir/$nn
mv $file $dir/$nn
sleep 1
echo " "
done
