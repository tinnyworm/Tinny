#!/usr/local/bin/perl -w

while(<>) {
	 @corrs = split /\s+/;
	 foreach (@corrs) {
		  chomp;
		  print "$_\n";
	 }
}
