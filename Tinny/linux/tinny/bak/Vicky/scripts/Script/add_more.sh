#! /bin/sh
# 
#  Add BIN mapping analysis over month and merchant ID analysis
#
#  yxc   05/04/04  creator
#
################################################################

#   for gathing the merchant length distrition plot

zz=/usr/local/bin/zcat
lenDis=/home/yxc/scripts/Perl/checkAuthsMKeyLength.pl
mkMatch=/home/yxc/scripts/Perl/checkAuthsMPMatch.pl 

#### handling arguments ####

test -d analysis || mkdir analysis

while [ $# -gt 0 ]
do

case "$1" in
   -a) options="$options   $1"
        auths=$2
### for auths
            echo "Generating the $auths 's AUTH-MRCH-ID length distibuion..."
            $zz $auths | $lenDis > analysis/merchantKey.lenDist
            sleep 5
				test -f mp.keys.gz || ln -s /work/fpcostello/asciiMP/FP_productionASCIIMP/MCUSA_2005/mp.keys.20050928.gz mp.keys.gz
            $zz $auths | $mkMatch -a - -m mp.keys.gz > analysis/merchantKey.match
            echo "$auths Merchant ID done"
            echo "generating hash saver for $auths..."
            multi_freq.pl -f$auths,1-6,20-25
     shift ;;
   -c) options="$options   $1"
       cards=$2
            echo "generating hash saver for $cards..."
            multi_freq.pl -f$cards,1-6,91-96
     shift ;;
   -f) options="$options   $1"
       frauds=$2
            echo "generating hash saver for $frauds..."
            multi_freq.pl -f$frauds,1-6,21-26 
       shift ;;
   -t) options="$options   $1"
       posts=$2
            echo "generating hash saver for $posts..."
            multi_freq.pl -f$posts,1-6,49-54
        shift ;;
   -p) options="$options   $1"
       paymnts=$2
            echo "generating hash saver for $paymnts..."
            multi_freq.pl -f$paymnts,1-6,82-87
       shift ;;
   -n) options="$options   $1"
       nonmons=$2
            echo "generating hash saver for $nonmons..."
            multi_freq.pl -f$nonmons,1-6,21-26
       shift ;;
    *) echo "wrong options $1" ;;
 esac
 sleep 5
 echo " "
 shift
 done


test -d BIN || mkdir BIN
mv hash*save BIN

ufal=/home/yxc/scripts/Perl//ufal_intersect.pl

#$ufal -a $auths -c $cards -f $frauds -t $posts -n $nonmons -p $paymnts
#-s0.1 > intersect
$ufal -a $auths -c $cards -f $frauds -s0.01 > intersect

echo "Intersection analysis results preview"
cat intersect | grep "in cards"
cat intersect | grep "in auths"
banner done
