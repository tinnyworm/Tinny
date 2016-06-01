#!/usr/local/bin/perl

use warnings;
use strict;

my $usage = "$0 date_to_match\n".
	"Writes out to STDOUT\n\n";

my $date = shift
	or die $usage;

open (INDEX, "zcat frauds.dat.gz | /work/price/falcon/Linux/bin/ufalcut -c1-19,21-28 | grep $date |")
	or die "Can't open frauds.dat.gz";

open (AUTH, "zcat auths.dat.gz |") 
	or die "Can't open auths.dat.gz";

my @match_date;

my $total_auth=0;
print "Extracting auth file";
while(<AUTH>) {
	print "   ...\b\b\b\b\b\b";
	my $auth_date = substr($_,19,8);
	if ($auth_date == $date) {
		$total_auth++;
		push @match_date, $_;
	}
}
close(AUTH);
print "\n $total_auth transactions in $date\n";

my $total_fraud = 0;
my $found=0;

while(<INDEX>) {
	$total_fraud++;
	my $acc = substr($_,0,19);
	print "\n finding match for $acc ...";
	foreach (@match_date) {
		my $auth_acc = substr($_,0,19);
		#print "$_\n";
		if ($acc == $auth_acc) {
			$found++;
			print "match!";
			last;
		}
	}
}

close(INDEX);

print "\n\n Out of total $total_fraud\n";
printf("%d (%6.2f%%) are found in auth\n", $found, $found/$total_fraud);
