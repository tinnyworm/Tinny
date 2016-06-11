#!/usr/bin/env perl

use warnings;
use strict;
use Getopt::Std;

# $Log: sample_rate_adjust.pl,v $
# Revision 1.1  2006/03/22 23:41:02  dxl
# Initial revision
#

my $usage = "\nUsage:\n".
            " cat <original risktab> | $0 \n".
            "     -s <original sampling rate, e.g. 1 for 1%> \n".
            "     -n <new sampling rate, e.g. 3 for 3%> \n".
            "     [-h] help \n".
            " > <new risktab>".
            "\n".
            "Adjust sampling rate for risk table\n\n";

my %opt = ();

die $usage unless ($#ARGV>0);
die $usage unless getopts("s:n:h", \%opt);
die $usage if($opt{h});

my $nTotal;

while(<>) {
    if(/(^.*),(\d+),(\d+),(.*)$/) { # $2--num of total, $3--num of fraud 
        $nTotal=int(($2-$3)*$opt{n}/$opt{s}+$3+0.5);
        print "$1,$nTotal,$3,$4\n";
    } else {
        print $_;
    }
}

