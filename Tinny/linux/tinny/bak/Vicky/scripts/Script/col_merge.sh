#! /bin/sh
# merge sampled data

zz=/usr/local/bin/gunzip
tes=/work/price/falcon/bin//testChecksum

test -d "merge" || mkdir merge

while [ $# -gt 0 ]
do

 case "$1" in
   -a) options="$options   $1"
        file=`echo "auth"`
        key=`echo "-c1-33"`
        filel=`echo "auths"`;;
#      shift ;;

   -c) options="$options   $1"
        file=`echo "card"`
        key=`echo "-c1-19,91-98"`
        filel=`echo "cards"`;;
#      shift ;;
      
   -t) options="$options   $1"
        file=`echo "post"`
        key=`echo "-c1-19,49-56"`
        filel=`echo "posts"`;;
#      shift ;;
      
  -p) options="$options   $1"
        file=`echo "pay"`
        key=`echo "-c1-19,82-89"`
        filel=`echo "paynmt"`;;
#    shift ;;
     
   -n) options="$options   $1"
       file=`echo "non"`
        key=`echo "-c1-19,21-28"`
        filel=`echo "nonmons"`;;
#     shift ;;
     
    -i) options="$options   $1"
       file=`echo "inq"`
        key=`echo "-c1-19,21-34"`
        filel=`echo "inq"`;;
#     shift ;;
     
   *) echo "wrong options $1" ;;
 esac
# sleep 5
 echo " "

echo "$file file merging..."
$zz -c datas/*$file* | wc -lc
/work/gold/bin/col_merge $key datas/*$file* | gzip -c > merge/$filel.dat.gz 
sleep 15
$zz -c merge/$filel.dat.gz | wc -lc
echo "merge/$filel.dat.gz"
sleep 2
# testing encryption ##
echo "testing the encryption of $filel"
$zz -c merge/$filel.dat.gz | $tes | sort  | uniq -c 
sleep 5
echo " "
  shift
done
banner done
