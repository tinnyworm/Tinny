#!/usr/local/bin/tcsh -f

set usage = "Usage: restype.sh <big score file> [-s <sampling rate>] <fraud type1> <fraud type2>...  - Generate type by type bigscore file and research file from input score file"

set error = 0;
if ($#argv < 2) then
  echo $usage > /dev/stderr
  set error = 1;
else if !(-r $1) then
  echo "Cannot open input bigscore file $1, exiting..."
  set error = 1;
endif

#if !(-X big2res) then 
#  echo "Please put big2res in path, exiting..."
#  set error = 1
#endif

set c = 2;
set rate = 0.01;

if ($error) exit;

set s = -s
if ("$2" == "-s") then
  set rate = $3;
  set c = 4;
endif

#strip off directory and .gz, outputs to current working directory
set outbase = $1:t:r

foreach type ($argv[$c-])
    echo "Working on $type..." > /dev/stderr
    #Note that EDS-TD uses all 19 digits for encrypted account number
    gunzip -c $1 | gawk -v t=$type '($6==0 || $9 ~ "[" t "]")' | gzip >! $outbase.$type.gz
    /work/gold/bin/big2res -s $rate $outbase.$type.gz $outbase.$type.res 
end
