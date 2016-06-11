#! /bin/sh/
# change name

for file in `ls 03*`
do
key=`echo $file| sed "s:\.: :g" | gawk '{print $4}'`
case "$key" in
00324) nn=`echo "auths___.20$file"`;;
00291) nn=`echo "cards___.20$file"`;; 
00318) nn=`echo "posts___.20$file"`;;
*)      echo "what is it? $file"; exit 1;;
esac
echo $file
echo $nn
mv $file $nn
sleep 1
echo " "
done
