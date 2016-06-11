#! /usr/local/bin/bash

echo "CF cards accounts:"; 
gzip -dc cards.dat.gz | gawk 'substr($0,177,2)=="CF" {print substr($0,1,19)}' | sort | uniq | wc -l ;
echo "";
echo "CF cards accts Not in Frauds But has auths transctions:"; 
gzip -dc cards.dat.gz | gawk 'substr($0,177,2)=="CF" {print substr($0,1,19)}' | sort | uniq |  fishUnsorted.pl -v -b frauds.dat.gz -c1-19 -s1 - | fishUnsorted.pl -b - -c1-19 -s1 auths.dat.gz | ufalcut -c1-19 | sort | uniq -c | wc -l; 
gzip -dc cards.dat.gz | gawk 'substr($0,177,2)=="CF" {print substr($0,1,19)}' | sort | uniq |  fishUnsorted.pl -v -b frauds.dat.gz -c1-19 -s1 - | fishUnsorted.pl -b - -c1-19 -s1 auths.dat.gz | gzip -c > auths_withSusFraud.gz
