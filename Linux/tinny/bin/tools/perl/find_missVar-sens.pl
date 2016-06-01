#!/usr/local/bin/perl -w

$usage = "cat sens1 | $0 sens2 > missingvars.sens\n".
		"finding missing variables in sens2, comparing to sens1\n";

$sens2 = shift(@ARGV);

die $usage unless $sens2;

# read in all the variables from sens2, save in a hash table
open(SENS2, "cat $sens2 |") or die "Can not open $sens2!\n";
while (<SENS2>) {
	@elements = split / /, $_;
	$vars{$elements[0]} = 1;
}
close(SENS2);

while (<>) {
	@elements = split / /, $_;
	if ( !defined($vars{$elements[0]}) ) {
		print $_;
	}
}	
		
