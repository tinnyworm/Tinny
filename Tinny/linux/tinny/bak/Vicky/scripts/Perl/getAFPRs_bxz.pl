#!/usr/local/bin/perl -I/home/bxz/bin/perlmod

##############################################
# usage added.

require "getopts.pl";

$DEBUG = 0;

$USAGE = "
	This program is to extract the information from Research file 
	and estimated the result for each AFPR point.

	Usage:	cat research file | $0 

			
		Optional:
			-h shows this message
\n\n";

#die "$USAGE" if ($#ARGV == -1);
die "$USAGE" if ($opt_h);

##############################################

use Interpolate;

while(<>){
  last if(/ACCOUNT BASED/);
}
while(<>){
  last if(/^-+$/);
}
while(<>){
  last if (/^\w/);
  if (/^\s*(\d+)\s+(\d+.\d+)\s+(\d+.\d+)\s+(\d+.\d+)\s+(\d+.\d+)\s+/){
    push @scores,$1;
    push @afprs,$2;
    push @adrs,$3;
    push @cl1,$4;
    push @cl2,$5;
    if ($1 == 1000) {
      $cl1_1k=$4;
      $cl2_1k=$5;
    }
  }else{
    next;
  }
}
printf " %5s, %5s, %5s, %5s, %5s, %5s\n","AFPR","Score","ADR","rtVDR","olVDR","K";
foreach $i qw(0 2 4 6 8 10 12 14 16 18 20 25 30 35 40 45 50 60 70 80 90 100){
  $out_sc=Interpolate::int_2pt(\@afprs,\@scores,$i);
  $out_ad=Interpolate::int_2pt(\@afprs,\@adrs,$i);
  $out_l1=($cl1_1k-Interpolate::int_2pt(\@afprs,\@cl1,$i))/$cl1_1k*100.0;
  $out_l2=($cl2_1k-Interpolate::int_2pt(\@afprs,\@cl2,$i))/$cl2_1k*100.0;
  $k=($i+1.0)*$out_ad/100.0;
  printf " %5.1f, %5.0f, %5.2f, %5.2f, %5.2f, %5.2f\n",$i,
        $out_sc,$out_ad,$out_l1,$out_l2,$k;
}
