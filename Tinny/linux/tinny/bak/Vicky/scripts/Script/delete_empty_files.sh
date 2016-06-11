#! /usr/local/bin/bash

dir=$1
true=$2

for file in `ls -l $dir/*gz | gawk '$4==20 {print $8}'`
do
ls -l $file
if [[ $true == Yes ]]; then
 /bin/rm $file
else 
  echo "No deleting action performed. Append "Yes" to your command if you want to do so"
fi

done
