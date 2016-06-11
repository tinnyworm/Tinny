#!/usr/local/bin/perl

# This is used to fixed the fraud type in CIBC data, which is column 20
# in the fraud file. It maps
#		" " --> "O"
#		"0" --> "L"  
#		"1" --> "S"
#		"2" --> "N"
#		"3" --> "A"
#		"4" --> "C"  
#		"5" --> "O"  
#		"6" --> "M"
#  
# $usage = "zcat frauds.dat.gz | fix_fraudType.pl | gzip -c >".
#			"frauds_fixed.dat.gz";

use warnings;
use strict;

my $fraud_type;

while(<STDIN>) {
	$fraud_type = substr($_,19,1);
	if ($fraud_type eq " ") {
		substr($_,19,1)="O";
	} elsif ($fraud_type eq "0") {
		substr($_,19,1)="L";
	} elsif ($fraud_type eq "1") {
		substr($_,19,1)="S";
	} elsif ($fraud_type eq "2") {
		substr($_,19,1)="N";
	} elsif ($fraud_type eq "3") {
		substr($_,19,1)="A";
	} elsif ($fraud_type eq "4") {
		substr($_,19,1)="C";
	} elsif ($fraud_type eq "5") {
		substr($_,19,1)="O";
	} elsif ($fraud_type eq "6") {
		substr($_,19,1)="M";
	}
	
	print $_;
}	
