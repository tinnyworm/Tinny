#! /usr/local/bin/perl

use warnings;
use lib "/home/dxl/perl/perllib";
use Date::Manip;

$usage = "usage: $0 date(yyyymmdd)\n\n".
         "Return 1 for Monday, ... 7 for Sunday";

$start_date = shift
	or die $usage;

($y,$m,$d) = unpack("a4 a2 a2",$start_date);

$dayofweek = Date_DayOfWeek($m,$d,$y);
	
print "$dayofweek\n";
