#! /usr/local/bin/perl

# look at the api file, compare the ACCOUNT_EXPIRE_DATE and
# CARD_EXPIRE_DATE, according to AUTH_KEYED_SWIPED

use warnings;
use strict;

my $usage = "$0 api_file\n";

my $api_file = shift
	or die $usage;

open(API, "zcat $api_file |")
	or die "Can't open $api_file"; 

my $all_total=0;
my $swiped_total=0;
my $keyed_total=0;
my $blank_total=0;
my $swiped_match=0;
my $keyed_match=0;
my $blank_match=0;

while(<API>) {
	my $acc_exp_date = substr($_,49,6);
	my $card_exp_date = substr($_,57,6);
	my $auth_keyed = substr($_,172,1);
	$all_total++;

	print "\r $acc_exp_date   $card_exp_date   $auth_keyed   $all_total";
	
	if ($auth_keyed eq "S") {
		$swiped_total++;
		if ($acc_exp_date eq $card_exp_date) {
			$swiped_match++;
		}
	} elsif ($auth_keyed eq "K") {
		$keyed_total++;
		if ($acc_exp_date eq $card_exp_date) {
			$keyed_match++;
		}
	} elsif ($auth_keyed eq " ") {
		$blank_total++;
		if ($acc_exp_date eq $card_exp_date) {
			$blank_match++;
		}
	}
}

printf("\n");
printf("Total transactions: %d\n", $all_total);
printf("Total swiped:         %d, %7.4f%%\n", $swiped_total, 
	100*$swiped_total/$all_total);
printf("Total keyed:         %d, %7.4f%%\n", $keyed_total, 
	100*$keyed_total/$all_total);
printf("Total blank:         %d, %7.4f%%\n", $blank_total, 
	100*$blank_total/$all_total);
printf("Matched swiped:       %d, %7.4f%%\n", $swiped_match, 
	100*$swiped_match/$swiped_total);
printf("Matched keyed:      %d, %7.4f%%\n", $keyed_match, 
	100*$keyed_match/$keyed_total);
printf("Matched blank:      %d, %7.4f%%\n", $blank_match, 
	100*$blank_match/$blank_total);


