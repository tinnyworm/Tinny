#! /usr/local/bin/perl

use warnings;
use lib "/home/dxl/perl/perllib";
use Date::Manip;

$usage = "usage: $0 start_date offset\n\n".
	"\t Return the date given the \"offset\" of the \"start_date\"\n\n".
	"\t start_date: yyyymmdd\n".
	"\t offset: 1day(s), 7days, 1month(s), 3months ...\n";

$start_date = shift
	or die $usage;
$offset = shift
	or die $usage;
	
#$date = &ParseDate($start_date);
$final_date = substr(&DateCalc($start_date,"$offset"),0,8);

print "$final_date\n";
