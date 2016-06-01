#! /usr/local/bin/perl

use lib "/home/oxb/bin/PM";
use Getopt::Std;

my $version = 0.1;

my $num_args = @ARGV;


getopts("b:vhp:t:");

if ($opt_v) {
  print " 3waysplit.pl :: Version - $version \n";
  exit;
}

if ($opt_h) {
  print " 3waysplit.pl     :: Version - $version \n";
  print "\n";
  print "Example: \n";
  print "3waysplit -t youfcr20 -b out/bScr.scale.gz -p 20 \n";
  print "\n";
  print " Command Line Arguments for the Script: \n";
  print "-b <bigscore file>  : big score file for the scale file (default is \"out/bScr.scale.gz\")\n";
  print "-p <percent test>   : percent of the data to be retag as test (default is 20. tis gives around %25 test data.)\n";
  print "-t <base name>      : base model name\n";
  print "-h                  : Print help information\n";
  print "-v                  : Print version of the program.\n";
  exit;
}

if ($opt_b) {
  $bigscr  = $opt_b;
} else {
  $bigscr = "out/bScr.scale.gz";
}


if ($opt_p) {
  $perc_test = $opt_p;
} else {
  $perc_test = 20;
}

if ($opt_t) {
  $base = $opt_t;
} else {
  die "base model name is missing \n";
}  


`mv $base.tag $base.tag.org`;

$tmp=`uname -s`; 
chomp $tmp;
if($tmp =~ /^Linux/){
	$hash="/work/price/falcon/Linux/bin/hashtagblind"; 
}else{
	$hash="/work/price/falcon/bin/hashtagblind"; 
}

`gunzip -cf $bigscr | $hash $perc_test 1> $base.ntag 2> $base.tmp1`;

open(IN, "tail -3 $base.tmp1 | ")  or die "cannot read from tmp1 file: $!\n";
open(OUT, "> $base.tag")  or die "cannot write to file $base.tag: $!\n";

$line = <IN>;
print OUT $line;
$line = <IN>;
print OUT $line;


close OUT;
close IN;

`cat  $base.ntag >> $base.tag`;

unlink "base.ntag", "tmp1";


