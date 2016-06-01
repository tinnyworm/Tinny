#!/bin/sh

echo "==========================================================="

for j in 0 500 550 600 650 700 750 800 850 900 950
do
    echo "Switch date = $1, Threshold = $j"
    /usr/local/bin/zcat ../eval/switch-2003$1/*.bigScr.gz \
      | ../bin/getNonFraudCases $j \
      | ../bin/dateFilter $2 $3 \
      | wc -l 
done
