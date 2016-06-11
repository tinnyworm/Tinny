#!/usr/local/bin/perl -w

$usage = "Find variables defined in vars1 but not in vars2".
			"vars1 and vars2 are obtained by 'grep ^VAR var-define files'\n\n".
			"\t cat vars1 | $0 vars2 > extra-vars.txt\n";

$vars2 = shift(@ARGV);

die $usage unless $vars2;

# read in all the variables from vars2, save in a hash table
open(VARS2, "cat $vars2 |") or die "Can not open $vars2!\n";
while (<VARS2>) {
	 @elements = split /\s+/, $_;
	 if (defined($elements[1])) {
		$vars{$elements[1]} = 1;
  }
}
close(VARS2);

while (<>) {
	@elements = split /\s+/, $_;
	 if (defined($elements[1])) {
		if ( !defined($vars{$elements[1]}) ) {
			print $_;
  		}
  }
}	
		
