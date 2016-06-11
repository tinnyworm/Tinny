#!/usr/local/bin/perl -w

#$usage = "Usage: \n\t" .
#			"zcat <bigScr file> | $0 tagfile \n";

$tagfile = shift(@ARGV);

open(TAG, "cat $tagfile |") or die "Can not open $tagfile file!\n";

while (<>) {
	 if (/test/) {
		  $a = <TAG>;
		  print $a;
	 }
}
