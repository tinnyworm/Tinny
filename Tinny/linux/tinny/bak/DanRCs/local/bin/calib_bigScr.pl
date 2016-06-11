#!/usr/bin/env perl

# $Log: calib_bigScr.pl,v $
# Revision 1.2  2006/04/27 06:19:40  dxl
# *** empty log message ***
#
# Revision 1.1  2006/04/25 21:36:17  dxl
# Initial revision
#

use strict;
use warnings;
use Getopt::Std;

my $usage = "\nUsage:\n".
            " zcat <bigScr> | $0 -t <CalibTab> [-h]\n\n".
            "$0 applies the calibration table on the big score files\n".
            "directly to generate the new scores for the model\n";

my %opt=();

die $usage unless ($#ARGV>0);
die $usage unless getopts("t:h",\%opt);
die if($opt{h});

open(TABLE, "<$opt{t}")
   or die "Can NOT open calibration table $opt{t}!\n";

my @table;

while(<TABLE>) {
    if(/^(\d+),(\d+)/) {
        $table[$1] = $2;
    }
}
close(TABLE);

while(<>) {
    my $scr=substr($_,83,4);
    my $nscr=sprintf("%4d",$table[$scr]);
    substr($_,83,4)=$nscr;
    print $_;
}
    
