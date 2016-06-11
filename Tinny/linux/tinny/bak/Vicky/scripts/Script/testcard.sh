zz=/usr/local/bin/zcat

for file in `ls *CARD*`
do 
   test=$zz $file |dd conv=ascii | addnl 186 | cut -c91-93 | uniq |grep '200'

case "$test" in  
   200) echo $file $test ;;    
   *)  echo "WRONG CARD FILE $file"; exit 1;; 
esac 

sleep 10 
done
