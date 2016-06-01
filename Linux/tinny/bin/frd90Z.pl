#! /usr/local/bin/perl -I/home/jaz/tools/perl/perlmod -wn
use D90;

($d1,$d2)=unpack("\@20 a8 \@35 a8",$_);
($dd1,$dd2)=(D90::today90($d1),D90::today90($d2));
$rd=$dd2-$dd1;
if($rd>90) {substr($_,19,1)='Z';}
print $_;
