#! /usr/local/bin/perl

use warnings;
use strict;

my $ca_total=0;
my $ca_valid=0;
my $us_total=0;
my $us_valid=0;
my $other_total=0;

while(<>) {
	my $zip = substr($_,88,3);
	my $cntry = substr($_,97,3);
	
	if ($cntry eq "124") {
		$ca_total++;
		if ($zip =~ /[A-Z]\d[A-Z]/) {
			$ca_valid++;
			# print "ca_valid: $zip\n";
		}
	} elsif ($cntry eq "840") {
		$us_total++;
		if ($zip =~ /\d\d\d/) {
			$us_valid++;
			# print "us_valid: $zip\n";
		}
	} else {
		$other_total++;
	}
}

my $all_total = $ca_total+$us_total+$other_total;
printf("Total transactions: %d\n", $all_total);
printf("Canada: %d, \t US: %d, \t Other: %d\n", $ca_total, $us_total,
	$other_total);
printf("Valid zip3 in Canada: %d, %5.2f%%, (%5.2f%% in total)\n", $ca_valid,
	100*$ca_valid/$ca_total, 100*$ca_valid/$all_total);
printf("Valid zip3 in Us: %d, %5.2f%% (%5.2f%% in total)\n", $us_valid,
	100*$us_valid/$us_total, 100*$us_valid/$all_total);
