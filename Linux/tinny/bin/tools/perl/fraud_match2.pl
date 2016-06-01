#!/usr/local/bin/perl

# find the number of frauds matching the transaction data

use warnings;
use strict;

my $usage = "$0 date\n".
		"Writes output to STDOUT\n\n";

my $date = shift
	or die $usage;

open (INDEX, "zcat frauds.dat.gz | /work/price/falcon/Linux/bin/ufalcut -c1-19,21-28 | grep $date |");

my $matchpattern = "'^.\\{19,26\\}$date'";

print "Finding pattern $matchpattern in auths ...   ";
`zcat auths.dat.gz | grep $matchpattern > auths.temp`;
print "Done\n";

my $total_fraud = 0;
my $notfound = 0;

while(<INDEX>) {
	$total_fraud++;
	my $acc = substr($_,0,19);
	$matchpattern = "'^.\\{0,18\\}$acc'";
	my $match = `grep $matchpattern auths.temp`;
	if (!$match) {
		print "no match for $acc\n";
		$notfound++;
	}
}

`rm -f auths.temp`;

printf("Out of total %d frauds\n", $total_fraud);
printf("%d (%6.2f%%) are found\n", $total_fraud-$notfound, ($total_fraud-$notfound)/$total_fraud);
	
