#!/usr/local/bin/perl

use warnings;
use strict;
use Getopt::Std;

# $Log: risk_table_defrisk.pl,v $
# Revision 1.1  2006/03/23 19:13:05  dxl
# Initial revision
#

my $usage = "\nUsage:\n".
            "\t$0 -t risktable [-h]\n\n".
            "\tPrint default riskrate (except SICxAMT entries)\n\n";

my %opt=();

die $usage unless ($#ARGV>0);
die $usage unless getopts("t:h",\%opt);
die $usage if($opt{h});

open(TABLE,"< $opt{t}") 
   or die "Can NOT open risktable $opt{t}!\n";

while(<TABLE>) {
    if(/^TABLE/ && !/SICxAMT/) {
        my @in = split / /;
        printf("%-40s",$in[1]);
        $_=<TABLE>; $_=<TABLE>;
        @in = split /,/;
        printf("%7.5f\n",$in[2]/$in[1]);
    } 
    
#    elsif (/^DEFAULT/) {
#        $_ = <TABLE>;
#        my @in = split /,/;
#        printf("%5.2f\n",$in[2]/$in[1]);
#    } 
}

close(TABLE);
