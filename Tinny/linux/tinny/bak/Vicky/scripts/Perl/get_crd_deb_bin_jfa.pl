#! /usr/bin/perl

require "getopts.pl";

$USAGE = "
Usage:	get_crd_deb_bin.pl [ -h ] -c<credit_BINs> -d<debit_BINs> -e<exclude_BINs>

		STDIN is the authorization hash file from multi_freq.pl and
		tablehash.pl using the BIN and credit/debit flag columns.
		BINs are separated into there respective output files.

		Required
			-c<credit_BINs>
			-d<debit_BINs>
			-e<exclude_BINs> BINs that have both credit AND debit flags

		Optional
			-h Show this message
";

die "$USAGE" if ($#ARGV == -1);
&Getopts('c:d:e:h');
die "$USAGE" if ($opt_h);
die "$USAGE" unless (($opt_c) && ($opt_d) && ($opt_e));

open (DBIN, ">$opt_d") || die "Unable to write to file: $opt_d\n";
open (CBIN, ">$opt_c") || die "Unable to write to file: $opt_c\n";
open (EBIN, ">$opt_e") || die "Unable to write to file: $opt_e\n";

$numTot = 0;
#$nonDeb = 0;
$numDeb = 0;
#$nonCrd = 0;
$numExc = 0;
$ccount = 0;
$dcount = 0;
$cbinid = -1;
$dbinid = -1;

$counter = 0;
while (<>) {
	$counter++;
	# Skip first line
	next if ($counter == 1);

	chomp;
	# Remove commas
	s/,//g;

	if (/^BIN=/) {
		$numTot++;

		if (($ccount > 0) && ($dcount > 0) && ($cbinid == $dbinid)) {
			$numExc++;
			print EBIN "$cbinid\n";
			warn "Near line $counter of bin.map: BIN $cbinid contains both credit and debit accounts\n"; 
		}
		elsif ($dcount > 0) {
			$numDeb++;
			print DBIN "$dbinid\n";
		}
		elsif ($ccount > 0) {
			$numCrd++;
			print CBIN "$cbinid\n";
		}
	}
	elsif (/^\s+(\d+)\s+1\s+(\d+)$/) {
		$dbinid = $1;
		$dcount = $2;
		#if ($2 != 0) {
			#$numDeb++;
			#print DBIN "$1\n";
		#} else {
			#$nonDeb++;
		#}
	}
	elsif (/^\s+(\d+)\s+0\s+(\d+)$/) {
		$cbinid = $1;
		$ccount = $2;
		#if ($2 != 0) {
			#$numCrd++;
			#print CBIN "$1\n";
		#} else {
			#$nonCrd++;
		#}
	}
	elsif ($_ ne "") {
		warn "Line $counter of bin.map does not match any pattern:  $_\n"; 
	}
}
close (DBIN);
close (CBIN);
close (EBIN);

printf ("%-15s%-15s%-15s%-15s\n", "Tot Num", "Num Crd", "Num Deb", "Num Excl");
printf ("%-15d%-15d%-15d%-15d\n", $numTot, $numCrd, $numDeb, $numExc);
