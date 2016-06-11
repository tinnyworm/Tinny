#!/usr/bin/env perl

use warnings;
use strict;
use Getopt::Std;
BEGIN { require ("/fo1/bin/perlmod/hashValue_ola.pm"); }

my $usage = "\nUsage:\n".
            "\t zcat api | $0 \n".
            "\t            -t train_pct(0-100)\n".
            "\t            -s start_hashValue(0-100)\n".
            "\t            -p output_prefix\n\n";

my %opt=();

die $usage unless ($#ARGV>0);
die $usage unless getopts("t:s:p:h",\%opt);
die $usage if($opt{h});

##############################

my $hashType = $hashValue::testTrain;
my $hashDirection = $hashValue::forward;

my $trainFile = "$opt{p}"."_train.dat.gz";
my $testFile = "$opt{p}"."_test.dat.gz";

open(TRAIN, "| gzip -c > $trainFile")
   or die "Can not open $trainFile to write";
open(TEST, "| gzip -c > $testFile")
   or die "Can not open $testFile to write";

my $startHash = $opt{s};
my $endHash = $startHash+$opt{t};

while (<>) {
   my $acct=substr($_,0,12);
   my $hashValue = &hashValue::hashValue ($acct,
                                       $hashType,
                                       $hashDirection);
   if($hashValue>=$startHash && $hashValue<$endHash) {
       print TRAIN $_;
   } else {
       print TEST $_;
   }
}

close(TRAIN);
close(TEST);
