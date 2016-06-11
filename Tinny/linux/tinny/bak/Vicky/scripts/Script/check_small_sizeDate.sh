#! /bin/sh


for file in `cat */0*/*list` 
do   
key=`znl -v -f $file | awk '{print $1}'`
if [ $key -gt 200 ] 
then 
     echo $key " " $file
     
fi 
done
