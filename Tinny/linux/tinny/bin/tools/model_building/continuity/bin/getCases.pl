#!/usr/local/bin/perl

# extract number of fraud and nonfraud for each switch week in continuity
# testing

$usage = " $0 extract the numbers of fraud and nonfraud for Dave's \n".
			" continuity test, and output the result in a csv file.\n\n".
			"\t Usage: $0 fraud.out nonfraud.out > cases.csv\n";

$fFraud = shift
	or die $usage;
$fNonFraud = shift
	or die $usage;

open(FRAUD, "cat $fFraud |")
	or die "Can not open $fFraud!";
open(NONFRAUD, "cat $fNonFraud |")
	or die "Can not open $fNonFraud!";

while(<FRAUD>) {
	 if(/Switch date/) {
		  ($p1,$p2) = split /,/;
		  @d = split /\s+/, $p1;
		  @t = split /\s+/, $p2;

		  $fraud{$t[-1]}{$d[-1]} = <FRAUD>+0;
	 }
}
close(FRAUD);

while(<NONFRAUD>) {
	 if(/Switch date/) {
		  ($p1,$p2) = split /,/;
		  @d = split /\s+/, $p1;
		  @t = split /\s+/, $p2;

		  $nonfraud{$t[-1]}{$d[-1]} = <NONFRAUD>+0;
	 }
}
close(NONFRAUD);

for $threshold (sort keys %fraud) {
	print "Threshod=$threshold\n";
	print "Week,Frauds,Nonfrauds\n";
	$i=0;
	for $week (reverse sort keys %{ $fraud{$threshold} }) {
		 print "$i,$fraud{$threshold}{$week},",
		 "$nonfraud{$threshold}{$week}\n";
		 $i++;
	}
	print "\n\n";
}


