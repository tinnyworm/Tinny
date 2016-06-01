#! /bin/sh
#


for file in `ls ./0*/merge/*gz`
do
echo $file
/usr/ucb/ln -s $file ./links/
done

/usr/ucb/ls -lL ./links
