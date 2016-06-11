#!/usr/local/bin/perl

use warnings;
use strict;
use Getopt::Std;

my $usage = "\nUsage:\n".
				"$0 <weekly counts> [-h]\n";

my $total_wk=$#ARGV;

my %opt = ();
die $usage unless ($total_wk>=0);
die $usage unless getopts("h", \%opt);
die $usage if($opt{h});

my %table=();

for(my $wk=0; $wk<=$total_wk; $wk++) {
	 open(IN,"<$ARGV[$wk]")
	 	or die "Can NOT open $ARGV[$wk]!\n";
    #print "$wk, $ARGV[$wk]\n";
	 while(<IN>) {
		  if(/^\s+(\d+),(\s+\d+,\s+\d+)/) {
            #print "$1, $_";
				$table{$1}[$wk]=$2;
		  }
	 }
	 close(IN);
}

foreach my $scr (sort keys %table) {
	 print "Threshold=$scr\n";
	 print "Week,     Frauds,  Nonfrauds\n";
	 for(my $wk=0; $wk<=$total_wk; $wk++) {
		  printf("%5d, %s\n", $wk,$table{$scr}[$wk]);
	 }
	 print "\n\n";
}
