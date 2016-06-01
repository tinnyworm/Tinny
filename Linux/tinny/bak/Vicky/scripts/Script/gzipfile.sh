#! /bin/rm
#  gzip files

zz=/usr/local/bin/zcat
for file in `ls  0303/*`
do
echo "$file is gzipping..."
dir=`echo $file | sed "s:\/: :" | gawk '{print $1}'`
filename=`echo $file | /home/yxc/test/namepart.py`
key=`echo $filename | sed "s:\.: :" | gawk '{print $1}'`
  case "$key" in
     authorization)    nn=`echo "$dir.auth.dat.gz"`;;
     cardholder)       nn=`echo "$dir.card.dat.gz"`;;
     posting)          nn=`echo "$dir.post.dat.gz"`;;
     *)                 echo "what is it ($file)?? ";;
  esac
echo $dir/$nn
mv $file $dir/$nn
sleep 2
echo " "
done
