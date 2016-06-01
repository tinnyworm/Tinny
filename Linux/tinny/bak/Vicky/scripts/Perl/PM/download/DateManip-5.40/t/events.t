#!/usr/local/bin/perl -w

require 5.001;
use Date::Manip;
@Date::Manip::TestArgs=();
$runtests=shift(@ARGV);
if ( -f "t/test.pl" ) {
  require "t/test.pl";
} elsif ( -f "test.pl" ) {
  require "test.pl";
} else {
  die "ERROR: cannot find test.pl\n";
}
$ntest=8;

print "1..$ntest\n"  if (! $runtests);
&Date_Init(@Date::Manip::TestArgs);

$tests ="

2000-02-01
   ~
   [
   2000020100:00:00
   [ Winter Event1 ] 
   ]

2000-04-01
   ~
   [
   2000040100:00:00
   [ Spring ] 
   ]

2000-04-01
0
   ~
   [
   2000040100:00:00
   [ Spring ]
   2000040112:00:00
   [ Spring Event3 ]
   2000040113:00:00
   [ Spring ]
   ]

2000-04-01 12:30
   ~
   [
   2000040112:30:00
   [ Spring Event3 ]
   ]

2000-04-01 13:30
   ~
   [
   2000040113:30:00
   [ Spring ]
   ]

2000-03-15
2000-04-10
   ~
   [
   2000031500:00:00
   [ Winter ]
   2000032200:00:00
   [ Spring ]
   2000040112:00:00
   [ Spring Event3 ]
   2000040113:00:00
   [ Spring ]
   ]

2000-03-15
2000-04-10
1
   ~
   {
   Event3 => +0:0:0:0:1:0:0
   Spring => +0:0:2:5:0:0:0
   Winter => +0:0:1:0:0:0:0
   }

2000-03-15
2000-04-10
2
   ~
   {
   Event3+Spring => +0:0:0:0:1:0:0
   Spring => +0:0:2:4:23:0:0
   Winter => +0:0:1:0:0:0:0
   }

";

print "Event_List...\n";
&test_Func($ntest,\&Events_List,$tests,$runtests);

1;
