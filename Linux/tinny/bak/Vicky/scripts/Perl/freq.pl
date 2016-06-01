#! /bin/env perl
#print "hi \n" ;

%binHash=qw() ;
%monHash=qw() ;
open(testfile,"zcat /work/gold/tape-processing/MBNA/FRAUDS/frauds.clean.200201_200309.gz|") ;
while ($line =<testfile>)
{
   chop $line ;
#   print "$line \n" ;
   $bin=substr($line, 0,6) ;
   $mon=substr($line,20,6) ;
#   print "$bin $mon \n" ;
   ($binHash{$bin} ++) or ($binHash{$bin} =1) ;
   ($monHash{$mon} ++) or ($monHash{$mon} =1) ;
}
close(testfile) ;

foreach $bin (keys %binHash) {print "$bin $binHash{$bin} \n" ;}
foreach $mon (keys %monHash) {print "$bin $monHash{$mon} \n" ;}

# print %binHash ; 
# print %monHash ;
