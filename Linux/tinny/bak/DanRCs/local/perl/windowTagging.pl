#!/usr/local/bin/perl 

use warnings;
use strict;
use Getopt::Std;

my $usage = "Usage:\n".
            "   zcat <tran file> | $0 -f <fraudTran file> [h]\n".
            "\n".
            "Find the first and last fraud tran datetime, \n".
            "Tag original <tran file> using 1 (inside fraud window) \n".
            " and 2 (outside fraud window)\n\n";
my %opt;

die $usage if ($#ARGV<1);
die $usage unless (getopts("f:h",\%opt));
die $usage if($opt{h});

my $fraud=$opt{f};

open FRAUD, "<$fraud" or die "Can NOT open $fraud\n";

my %fraudWindow;
my $preAcct="";

while(<FRAUD>) {
    my($acct,$dt)=unpack("a12 a14",$_);
    if($acct ne $preAcct) {
        $fraudWindow{$acct}[0]=$dt;
        $fraudWindow{$acct}[1]=$dt;
    } else {
        $fraudWindow{$acct}[1]=$dt;
    }
    $preAcct=$acct;
}

close(FRAUD);

while(<>) {
    my($acct,$dt) = unpack("a12 a14", $_);
    if(exists($fraudWindow{$acct})) {
        if($dt<$fraudWindow{$acct}[0] || $dt>$fraudWindow{$acct}[1]) {
            substr($_,1583,1)="2";
        } else {
            substr($_,1583,1)="1";
        }
    }
    print $_;
}
