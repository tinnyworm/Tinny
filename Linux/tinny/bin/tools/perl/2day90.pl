#! /usr/local/bin/perl 

use warnings;
use lib "/home/dxl/perl/perllib";
use Date::Manip;

$usage = "usage: $0 date(yyyymmdd)\n";

$date=shift
	or die $usage;

($y,$m,$d)=unpack("a4 a2 a2",$date);

$start = Date_DaysSince1BC(1,1,1990);
$now = Date_DaysSince1BC($m,$d,$y);

$day90 = $now-$start;

print "$day90\n";
