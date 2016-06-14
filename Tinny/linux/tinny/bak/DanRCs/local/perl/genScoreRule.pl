#!/usr/local/bin/perl

use warnings;
use strict;
use Getopt::Std;

my $usage = "$0 -c nnet_config_file -l logreg/linreg logfile [-h] > score.rul\n".
            "\n".
            "Generate scoring rule, using config file and logreg/linreg ".
            "log file.\n\n";

my %opt=();

die $usage unless ($#ARGV>0);
die $usage unless getopts("c:l:h",\%opt);
die $usage if($opt{h});

open LOG, "<$opt{l}" or die "Can NOT open log file $opt{l}";
open CFG, "<$opt{c}" or die "Can NOT open config file $opt{c}";

my %varlist=();
my $interc;
while(<LOG>) {
    last if(/Parameter Estimates/);
}
while(<LOG>) {
    if(/^(\w+)\s+\|\s+(\S+)/) {
        my $name=$1;
        my $param=$2;
        if($name ne "intercept") {
            $varlist{$name}=$param;
        } else {
            $interc=$param;
        }
    }
    last if(/Model Fit Chi-Square Test/);
}
close(LOG);

print "function scoring : float;\n";
print "var\n";
print "   tmp_scr : float;\n";
foreach my $var (keys %varlist) {
    printf("   z_%s : float;\n",$var);
}
print "begin\n";

$/="";
while(<CFG>) {
    if(/Name=\s*(\w+)\n/) {
        my $name = $1;
        if(exists($varlist{$name}) && (/Mean=\s*(\S+)\nStdDev=\s*(\S+)/)) {
            printf("   z_%s := (%s-%f)/%f;\n", $name, $name, $1, $2);
        }
        
    }
}
$/="\n";
close(CFG);

printf("\n");
printf("   tmp_scr := %f\n", $interc);
foreach my $var (keys %varlist) {
    printf("      + (%f * z_%s)\n", $varlist{$var}, $var);
}
printf("   ;\n\n");
printf("   tmp_scr := 1000/(1+exp(-tmp_scr));\n");
printf("   tmp_scr := max(1,min(tmp_scr,999));\n");
printf("\n");
printf("   return(tmp_scr);\n");
printf("endrule;\n\n");