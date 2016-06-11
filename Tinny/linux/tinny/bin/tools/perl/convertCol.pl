#!/usr/local/bin/perl -w

while(<>) {
	 @corrs = split /\s+/;
	 foreach $c (@corrs) {
		  chomp($c);
		  print "$c\n";
	 }
}
