#!/usr/local/bin/perl -w

$usage = "cat sens | $0 prof_map > nonprof_vars.sens\n".
		"finding nonprofile variables in sens\n";

$map = shift(@ARGV);

die $usage unless $map;

# read in all the variables from map, save in a hash table
open(MAP, "cat $map |") or die "Can not open $map!\n";
while (<MAP>) {
	@elements = split /,/, $_;
	# remove spaces
	$elements[2] =~ tr/ //d;
	# change to lowercase ASCII
	$elements[2] =~ tr/A-Z/a-z/;
	
	#print "!",$elements[2], "!\n";
	$vars{$elements[2]} = $elements[2];
}
close(MAP);

while (<>) {
	@elements = split / /, $_;
	# remove spaces
	$elements[0] =~ tr/ //d;
	# change to lowercase ASCII
	$elements[0] =~ tr/A-Z/a-z/;
	
	#print "!",$elements[0], "!\n";
	if ( !defined($vars{$elements[0]}) ) {
		print $_;
	}
}	
