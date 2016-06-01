#!/usr/local/bin/bash

if [ -z $2 ] 
then
  echo "Usage: $0 <basename> <varlist_file>"
  exit
fi

net=$1
varlist=$2


FBIN="/work/price/falcon/`uname`/bin/"

nvar=`grep -v '^[ \t]*$' $varlist|wc -l`
nvarp1=`expr $nvar + 1`

cnt_train=`gzip -l *train.prescl.gz|awk "{a=\\$2}END{print a/4/$nvarp1}"`
cnt_stop=`gzip -l *stop.prescl.gz|awk "{a=\\$2}END{print a/4/$nvarp1}"`

echo "Number of Training examples: $cnt_train"
echo "Number of Stopping examples: $cnt_stop"

$FBIN/create_tag.pl $cnt_train $cnt_stop > $net.tag

gzip -dc *train.prescl.gz *stop.prescl.gz\
|$FBIN/scaling -z nozerocalc,binary -l $varlist -b - -C $net.cfg
gzip -dc *train.prescl.gz *stop.prescl.gz\
|$FBIN/scaling -b - -c $net.cfg -o $net.scl
