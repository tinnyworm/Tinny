#! /bin/sh
#  standard name

zz=/usr/local/bin/zcat
zort=`echo "bmsort -T /tmp -S"`

#for file in `ls 0210/*.gz`
#do 
#   #pre=`echo $file | sed "s:\/: :" |awk '{print $1}'`
#   post=`echo $file | sed "s:\/: :" |awk '{print $2}'`
#   nn=`echo 20$post`
#   mv $file 0210/$nn
#done

for file in `ls 02*/2002*.gz`
do 
   direc=`echo $file | sed "s:\/: :g" | awk '{print $1}'`
   filename=`echo $file | sed "s:\/: :g" | awk '{print $2}'`
   key=`echo $filename | sed "s:\.: :g" | awk '{print $4}'`
   case "$key" in 
     00146)  nname=`echo "auths.$filename"`;;
     00186)  nname=`echo "cards.$filename"`;;
     *)        echo " what is it", exit 1;;
  esac
    
  # head=`echo $filename | sed "s:\.: :" | awk '{print $1}'`
  # main=`echo $filename | sed "s:\.: :" | awk '{print $2}'`
  # case "$head" in
  #  a*)   nname=`echo "auths.$main"`;;
  #   c*)   nname=`echo "cards.$main"`;;
  #   *)    echo "what is it?"; exit 1;;
  # esac
 
  mv $file $direc/$nname
done
    



#   key=`echo $file | sed "s/.gz/.dat.gz/g"`
#   mv $file $key
#done
