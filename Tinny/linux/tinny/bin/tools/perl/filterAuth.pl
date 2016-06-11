#! /usr/local/bin/perl -w

$usage = "$0 dir [legit|fraud]\n".
  "Writes out auth to STDOUT\n\n";

$dir = shift
  or die $usage;

$opt = shift
  or die $usage;

open(AUTH, "zcat $dir/auths.dat.gz |")
  or die "Can't open $dir/auths.dat.gz";
open(FRAUD, "zcat $dir/frauds.dat.gz |")
  or die "Can't open $dir/frauds.dat.gz";
$auth = <AUTH>;
while(<FRAUD>){
  $facc = substr($_,0,19);
  while($auth and substr($auth,0,19) le $facc){
    if($opt eq "legit" and substr($auth,0,19) lt $facc){
      print $auth;
    } elsif($opt eq "fraud" and substr($auth,0,19) eq $facc){
      print $auth;
    }
    $auth = <AUTH>;
  }
}
close(AUTH);
close(FRAUD);
