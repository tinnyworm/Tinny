#!/usr/bin/env perl

use warnings;
use strict;
use Getopt::Std;

# $Log: risk_frate_adjust.pl,v $
# Revision 1.1  2006/04/10 21:06:53  dxl
# Initial revision
#

# Adjust overall fraud rate through sampling rate
# R_current = #F/(#NF+#F)
# R_target = #F/(#NF*a+#F)
# ==> a=((1-R_target)/R_target)/((1-R_current)/R_current)

my $usage = "\nUsage:\n".
            " cat <original risktab> | $0 \n".
            "     -c <current overal fraud rate, e.g. 0.01> \n".
            "     -t <targeted overal fraud rate> \n".
            "     [-h] help \n".
            " > <new risktab>".
            "\n".
            "Adjust overall fraud rate for risk table to the \n".
            "targeted overall fraud rate R_t. In order to achieve \n".
            "better continuity, R_t should be the overall fraud rate\n".
            "for the old risktable.\n".
            "E.g. cat risktab | $0 -r 0.03 -t 0.02 > new_risktab\n\n";

my %opt = ();

die $usage unless ($#ARGV>0);
die $usage unless getopts("c:t:h", \%opt);
die $usage if($opt{h});

my $nTotal;
my $a=((1-$opt{t})/$opt{t})/((1-$opt{c})/$opt{c});

while(<>) {
    if(/(^.*),(\d+),(\d+),(.*)$/) { # $2--num of total, $3--num of fraud 
        $nTotal=int(($2-$3)*$a+$3+0.5);
        print "$1,$nTotal,$3,$4\n";
    } else {
        print $_;
    }
}

