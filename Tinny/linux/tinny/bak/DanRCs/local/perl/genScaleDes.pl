#!/usr/bin/env perl

# Add line number, start from 1

use warnings;
use Getopt::Std;

$usage = "Usage:\n".
         "   cat varlist | $0\n\n".
			"Generate variable description file for genScaleTable.pl. ".
			"Take the variable list and add line number and R for the ".
			"category\n";

getopts("h");

die $usage if($opt_h);

$n=0;

while(<>) {
    print "$n\tR\t$_";
    $n++;
}
