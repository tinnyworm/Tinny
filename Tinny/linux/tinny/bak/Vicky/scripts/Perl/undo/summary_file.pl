#! /usr/local/bin/perl -w

###### This program is to summary the raw data size ######
##########################################################

<< "xxxEOFxxx";
Usage: $0 [-m 0504:0506]
			 
Purpose:	 
	To summarize the size by month and by type. 

Arguments:
	[-m]: Please specify the month start and end.
	
Options:
				
Input:
	ls -l all files

Output:
	Report by month/by type 

Example:

	ls -l *gz | $0 -m 200504:200507 
		
Revision History:
	Authors			Date			Involvement
	yxc			 8/16/2005      Creator
		 
Note: 
	
xxxEOFxxx

################################################################################
print STDERR "\n";
print STDERR "Command line: $0 @ARGV\n";
print STDERR "Started on " . scalar localtime() . "\n\n";

################################################################################

use Getopt::Std;

$opt_m = "";

getopts('h:m:');
$err=""; 

if (!$opt_m) {
	$err .= "Must specify the month start and end if desired.\n"; 
	$opt_h = 1; 
}
if ($opt_h){
	print STDERR $err;
	print STDERR $usage;
	exit (1); 
}


@temp_m=split(":", $opt_m); 
$start=$temp_m[0];
$end=$temp_m[1]; 
