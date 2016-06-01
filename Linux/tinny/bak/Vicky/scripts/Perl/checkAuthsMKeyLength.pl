#!/usr/local/bin/perl -w
################################################################################
#
#   Purpose: To get the length distribution of merchant key for FP
#
#   arguments: -h -n
#
#   input    : stdin falcon auths format data
#
#   Example  : zcat auths | checkAuthsMKeyLength.pl 
#
#   Revision :  pcd   01/21/2004   creator
#               yxc   10/04/2004   modifier
#
################################################################################

################################################################################
$usage = << "xxxEOFxxx";
Usage: $0 [-h] [-n]

Read auths from stdin.  Get the MCC (SIC) and the Merchant ID.  Examine
the length of the full key with training blanks removed and just the MID
with leading and training blanks removed.

    -n   Use the last 16 of Merch Name instead of Merch ID

    Columns:
       MCC               85 -  88
       MID              124 - 139
       MName (Last16)   189 - 204

Example  : zcat auths | checkAuthsMKeyLength.pl    

xxxEOFxxx
################################################################################

use Getopt::Std;
$opt_h = 0;  # Eliminate annoying warning.
$opt_n = 0;  # Eliminate annoying warning.
getopts ('hn');
if ($opt_n) {
    print "Using last 16 of Merchant Name\n";
    $unpackString = "\@84a4 \@188a16";
} else {
    print "Using Merchant ID\n";
    $unpackString = "\@84a4 \@123a16";
}
if ($opt_h) {
    print "$usage";
    exit 1;
}

################################################################################

$total = 0;
while (<>) {
    # ($mcc, $mid) = unpack "\@84a4 \@123a16", $_;
    ($mcc, $mid) = unpack $unpackString, $_;

    $mKey = "$mcc$mid";
    $mKey =~ s/\s+$//o;
    $mKeyLength = length ($mKey);

    $mid =~ s/\s+$//o;
    $mid =~ s/^\s+//o;
    $midLength  = length ($mid);

    $values = sprintf "%2d %2d", $mKeyLength, $midLength;
    $keyHist{$values}++;
    $total++;
}

################################################################################

$cum=0;
print "Table of mkey-length x mid-length\n\n";
#print " --count----   -cumulative-   --percent---   --cum-pct---   values\n";

############ yxc revised on the title ################

print " MCC+MID MID   ---count----   -cumulative-   --percent---   --cum-pct---\n";
print " -----------   ------------   ------------   ------------   ------------\n";
foreach $i (sort keys %keyHist) {
    $cum += $keyHist{$i};
    $pct = 100 * $keyHist{$i} / $total;
    $cumPct = 100 * $cum / $total;

#    printf " %11d   %12d   %12.4f   %12.4f    %s\n",
#            $keyHist{$i}, $cum, $pct, $cumPct, $i;

############ yxc revised on the print format ################

       printf "%11s  %11d   %12d   %12.4f%%   %12.4f%%\n", $i, $keyHist{$i}, $cum, $pct, $cumPct;
}


