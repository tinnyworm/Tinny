#!/usr/local/bin/perl

use warnings;
use strict;
use Getopt::Std;

my $usage = "$0 [-h] <research files>\n\n";

my %opt=();

die $usage unless ($#ARGV>0);
die $usage unless getopts("h",\%opt);
die $usage if($opt{h});

while((scalar @ARGV)>0) {
    my $res=shift @ARGV;
    open RES, "<$res"
       or die "Can not open $res";

    my($tAuth,$tFAuth,$aFAuth,$tAcct,$tFAcct);

    while(<RES>) {
        if(/^Total/) {
            if($_=~/^\D*(\d+),\D*(\d+),\D*(\d+)/) {
                $tAuth=$1;
                $tFAuth=$2;
                $aFAuth=$3;
            } elsif($_=~/^Total Number of Accounts:\s+(\d+)/) {
                $tAcct=$1;
            } elsif($_=~/^Total Number of Fraudulent Accounts:\s+(\d+)/) {
                $tFAcct=$1;
            } else {
                warn "Is $res a research file?";
            }
        }
    }
    close(RES);

    printf("%s,%d,%d,%d,%d,%d\n",$res,$tAuth,$tFAuth,$aFAuth,$tAcct,$tFAcct);
}

