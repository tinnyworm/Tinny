#!/usr/local/bin/tcsh -f

set res = $argv[1]
res2excel ../$res
res2scrdist.pl ../$res dist.${res}.csv
cat afpr.${res}.csv | head -n28 | tail -n22 | perl -wne 'chomp;s/%//g;@data = split /,/;$k = sprintf("%.2f", ($data[0] +1)*($data[2]/100));print "$data[0],$data[1],$data[2],$data[6],$data[7],$k\n";' > excel.$res.csv
echo "" >> excel.$res.csv
echo "" >> excel.$res.csv
echo "" >> excel.$res.csv
paste scr.${res}.csv dist.$res.csv |  perl -wne 'chomp;s/%//g;s/\t/,/g;@data = split /,/;$k = sprintf("%.2f", ($data[1] +1)*($data[2]/100));print "$data[0],$data[1],$data[2],$data[6],$data[7],$k,$data[13]\n";' >> excel.$res.csv


