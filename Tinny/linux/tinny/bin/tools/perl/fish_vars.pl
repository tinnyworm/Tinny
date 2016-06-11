#!/usr/local/bin/perl -w

$usage = "Usage: cat varlist | $0 <bait_file>\n\n".
			"Find variables in varlist which are also defined in the bait_file".
			"\n".
			"Varlist is obtained by 'grep ^VAR var-define files'\n".
			"Bait_file contains list of variables, each per line.";

$bait = shift(@ARGV);

die $usage unless $bait;

# read in all the variables from bait, save in a hash table
open(BAIT, "cat $bait |") or die "Can not open $bait!\n";
while (<BAIT>) {
	 @elements = split /\s+/, $_;
	 if (defined($elements[0])) {
		$vars{$elements[0]} = 1;
  }
}
close(BAIT);

while (<>) {
	@elements = split /\s+/, $_;
	 if (defined($elements[1])) {
		if ( defined($vars{$elements[1]}) ) {
			print $_;
  		}
  }
}	
		
