#!/usr/local/bin/perl -w

$usage = "$0 vars1 vars2\n".
		"find the missing variables in vars1 comparing to vars2.\n".
		"print to standard output.\n";

$vars1 = shift
	or die $usage;
$vars2 = shift
	or die $usage;	

open(VARS1, "cat $vars1 |")
	or die "Can't open $vars1\n";
open(VARS2, "cat $vars2 |")
	or die "Can't open $vars2\n";

# make list of variables in vars1
while (<VARS1>) {
	if(/^VAR  /) {
		#print $_;
		@content = split / /, $_;
		$content[2] =~ tr/ //d;
		#print $content[2], "\n";
		$varlist2{$content[2]} = $_;
	}
}
close(VARS1);

# check and print the missing variables comparing to vars2
$readgrp = 0;
while (<VARS2>) {
	if(/^VAR  /) {
		#print $_;
		@content = split(/ /,$_);
		$content[2] =~ tr/ //d;
		#print $content[1], "\n";
		if ( !defined($varlist2{$content[2]}) ) {
			print $content[2], "\t";
			
			$readgrp = 1;
		}
	} elsif ($readgrp) { # print out group info
		@groupline = split(/ = /, $_);
		print $groupline[1];

		$readgrp = 0;
	}	
}
close(VARS2);

