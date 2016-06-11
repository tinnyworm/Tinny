#! /usr/bin/perl -w

use lib ".";
use SimpleVarTrans;

###########################################################
# CutOff Test
###########################################################

my @patterns = ();
push( @patterns, '^\s*$');
push( @patterns, '^\s*0+\s*$');
push( @patterns, '^\s*9+\s*$');

my @range = ();
@range = qw(2 5 10);

my $strVal;
my $rVal;

$strVal = "";
$rVal = cutOffBin( $strVal, \@patterns, \@range );
print "Empty: 0 ==> $rVal\n";

$strVal = "00000";
$rVal = cutOffBin( $strVal, \@patterns, \@range );
print "$strVal: 1 ==> $rVal\n";

$strVal = "99999";
$rVal = cutOffBin( $strVal, \@patterns, \@range );
print "$strVal: 2 ==> $rVal\n";

$strVal = "00001";
$rVal = cutOffBin( $strVal, \@patterns, \@range );
print "$strVal: 3 ==> $rVal\n";

$strVal = "00002";
$rVal = cutOffBin( $strVal, \@patterns, \@range );
print "$strVal: 4 ==> $rVal\n";

$strVal = "00003";
$rVal = cutOffBin( $strVal, \@patterns, \@range );
print "$strVal: 4 ==> $rVal\n";

$strVal = "00004";
$rVal = cutOffBin( $strVal, \@patterns, \@range );
print "$strVal: 4 ==> $rVal\n";

$strVal = "00005";
$rVal = cutOffBin( $strVal, \@patterns, \@range );
print "$strVal: 5 ==> $rVal\n";

$strVal = "00009";
$rVal = cutOffBin( $strVal, \@patterns, \@range );
print "$strVal: 5 ==> $rVal\n";

$strVal = "00010";
$rVal = cutOffBin( $strVal, \@patterns, \@range );
print "$strVal: 6 ==> $rVal\n";

$strVal = "00999";
$rVal = cutOffBin( $strVal, \@patterns, \@range );
print "$strVal: 6 ==> $rVal\n";

###########################################################
# Pattern Test
###########################################################

print "\n\n";

@patterns = ();
push( @patterns, '^\s*$');
push( @patterns, '^\s*0+\s*$');
push( @patterns, '^\s*9+\s*$');

$strVal = "";
$rVal = patternMap( $strVal, \@patterns );
print "Empty: 0 => $rVal\n";

$strVal = "00000";
$rVal = patternMap( $strVal, \@patterns );
print "$strVal: 1 => $rVal\n";

$strVal = "99999";
$rVal = patternMap( $strVal, \@patterns );
print "$strVal: 2 => $rVal\n";

$strVal = "aaaaa";
$rVal = patternMap( $strVal, \@patterns );
print "$strVal: 3 => $rVal\n";
