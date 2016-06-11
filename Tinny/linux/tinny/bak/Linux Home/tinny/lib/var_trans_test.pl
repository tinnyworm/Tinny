#! /usr/bin/perl -w

use lib ".";
use VarTrans;

###########################################################
# CutOff Test
###########################################################
print "\n";
print "CutOff Test\n";

@blank_pat = qw(^\s*$        BLANK);
@zeros_pat = qw(^\s*0+\s*$   ALLZEROS);
@nines_pat = qw(^\s*9+\s*$   ALLNINES);

my @patterns = ();
push( @patterns, \@blank_pat );
push( @patterns, \@zeros_pat );
push( @patterns, \@nines_pat );

my @range = ();
@range = qw(2 5 10);

my $strVal;
my $rVal;

$strVal = "";
$rVal = cutOffBin( $strVal, \@patterns, \@range );
print "Empty: ==> $rVal\n";

$strVal = "00000";
$rVal = cutOffBin( $strVal, \@patterns, \@range );
print "$strVal: ==> $rVal\n";

$strVal = "99999";
$rVal = cutOffBin( $strVal, \@patterns, \@range );
print "$strVal: ==> $rVal\n";

$strVal = "00001";
$rVal = cutOffBin( $strVal, \@patterns, \@range );
print "$strVal: ==> $rVal\n";

$strVal = "00002";
$rVal = cutOffBin( $strVal, \@patterns, \@range );
print "$strVal: ==> $rVal\n";

$strVal = "00003";
$rVal = cutOffBin( $strVal, \@patterns, \@range );
print "$strVal: ==> $rVal\n";

$strVal = "00004";
$rVal = cutOffBin( $strVal, \@patterns, \@range );
print "$strVal: ==> $rVal\n";

$strVal = "00005";
$rVal = cutOffBin( $strVal, \@patterns, \@range );
print "$strVal: ==> $rVal\n";

$strVal = "00009";
$rVal = cutOffBin( $strVal, \@patterns, \@range );
print "$strVal: ==> $rVal\n";

$strVal = "00010";
$rVal = cutOffBin( $strVal, \@patterns, \@range );
print "$strVal: ==> $rVal\n";

$strVal = "00999";
$rVal = cutOffBin( $strVal, \@patterns, \@range );
print "$strVal: ==> $rVal\n";

###########################################################
# Pattern Test
###########################################################

print "\n\n";
print "Pattern Test\n";

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
