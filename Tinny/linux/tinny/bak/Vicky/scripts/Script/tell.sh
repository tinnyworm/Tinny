#! /bin/sh
# tell cons and mons

for file in `ls */*00049.hdr`
do 
   key=`cat $file | sed "s:\.: :g" | awk '{print $4}'`
   key2=`cat $file | head -2 | tail -1 | sed "s:\.: :g" | awk '{print $3}'`
   datafile=`echo $file | sed "s:\.: :g" | \
             awk '{print $1 "."  $2 "."  $3 "."  $4 ".*gz" }'`
   case "$key" in
     CONS)        echo "CONS- $file"; cp $datafile CONS/. ;;
     MTHLY)       echo "MONS- $file"; cp $datafile MONS/. ;;
     *)   case "$key2" in 
                 CONS)   echo "CONS: $file"; cp $datafile CONS/. ;;
		 MTHLY)  echo "MONS: $file"; cp $datafile MONS/. ;;
		 *)      echo "weird: $file";;
          esac
   esac
done   
