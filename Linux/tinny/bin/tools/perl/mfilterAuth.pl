#! /usr/local/bin/perl

# separate legitimate and fraud trans.
# when using [legit], output all the auth info about legitimate trans.
# when using [fraud], output all the auth info fraud trans.
#
# note: assuming the auths.dat.gz is arranged according to the card ID in
# ascend order

use warnings;

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
# the outer loop is going through the fraud file
while(<FRAUD>){
  $facc = substr($_,0,19);
  # the inner loop checks auth file according to the current fraud
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
