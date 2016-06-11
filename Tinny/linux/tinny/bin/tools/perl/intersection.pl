#!/usr/local/bin/perl

use warnings;
use strict;

my $usage = "$0 index_file search_file\n".
	"Writes out to STDOUT\n\n";

my $ind_file = shift
	or die $usage;

my $s_file = shift
	or die $usage;

open (INDEX, "$ind_file")
	or die "Can't open $ind_file";

my $total_fraud = 0;
my $found=0;
my $notfound=0;

while(<INDEX>) {
	$total_fraud++;
	my $acc = substr($_,0,19);
	print "finding match for $acc\n";
	my $match = `grep $acc $s_file`;
	if ($match) {
		$found++;
		print "$match\n";
	} else {
		$notfound++;
		print "no match for $acc\n";
	}
}

print "Out of total $total_fraud\n".
	  "$found are found in auth";
printf("\t%6.2f%%\n", $found/$total_fraud);
print "$notfound are not found\n";
