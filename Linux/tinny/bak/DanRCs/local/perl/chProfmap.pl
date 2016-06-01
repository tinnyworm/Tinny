#!/usr/bin/env perl

use warnings;
use strict;
use Getopt::Std;

# $Log: chProfmap.pl,v $
# Revision 1.1  2006/03/24 21:29:29  dxl
# Initial revision
#

my $usage = "\nUsage:\n".
            " cat <original var file> | $0 \n".
            "     -o <offset> e.g. 128, or -4 \n".
            "     -r <range>  e.g. 14-68 \n".
            "     -f <new var file> \n".
            "     [-h] help \n".
            "\n".
            "Change profile mapping, any mapping <n> within <range>\n".
            "will becomes <n+offset>\n\n";

my %opt=();

die $usage unless ($#ARGV>0);
die $usage unless getopts("o:r:f:h", \%opt);
die $usage if($opt{h});

my ($offset,$min,$max);

$offset = $opt{o};
($min,$max) = split /-/,$opt{r};

open(OUT,"> $opt{f}")
   or die "Can NOT open $opt{f} to write!\n";

while(<>) {
    if(/.*MAP.*=.*PROF.P(\d+)/) {
        if($1>=$min && $1<=$max) {
            my $nmap = $1+$offset;
            chomp;
            my $old = $_;
            $_ =~ s/$1/$nmap/;
            print(STDERR "$old ==> $_\n");
            print(OUT "$_\n");
        } else {
            print OUT $_;
        }
    } else {
        print OUT $_;
    }
}

close(OUT);
