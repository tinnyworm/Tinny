#!/usr/local/bin/perl -w
# -d:DProf
#
################################################################################
$usage = << "xxxEOFxxx";
Usage: $0 [-t <len>] [-l] [-c] -m <mpKeys> -a <auths>

Given the keys for merchant profiles (may be the full ascii merch prof
files, but does not need to be) and auths data, determine the matching
rates.

  -m <mpKeys> Merchant Profile Keys in columns 1-19 of file <mpKeys>
  -a <auths>  Consortium auths with MCC in 85-88, MID in 124-139
  -t <len>    Truncate the **Merchant KEY**  to <len> bytes.
  -l          Left justify the MID in the auths (move leading spaces to the end)
  -c          Cash and Merchandise only (auth col 64 = [CM]).

For any file, "-" implies stdin.

Merchant Key might be located at  /work/fpcostello/asciiMP/FP_productionASCIIMP/MCW_wk90_0729_0781/. 

Suggested use when both MP's and Auths in are in multiple files:

  \$  mknod mpIn p
  \$  gzip -dc mp.ascii.0689.*gz > mpIn &
  \$  gzip -dc 200303*AUTH*gz | $0  -a - -m mpIn

Suggested use when MP's are in multiple files but auths are in one file:

  \$  gzip -dc mp.ascii.0689.*gz | $0  -a auths.dat.gz -m -

Suggested use when auths's are in multiple files but MP's are in one file:

  \$  gzip -dc auths.*.gz | $0  -a - -m mpKeys.gz

xxxEOFxxx
#
# Author: pcd
# Date: 5/14/2004
# 
# Modifier: yxc on the print format
# Date: 10/22/2004
################################################################################

# $DEBUG = 1;

################################################################################

$MAX_KEY_LEN = 19;

################################################################################
print "\n";
print "Command line: $0 @ARGV\n";
print "Started on " . scalar localtime() . "\n\n";

################################################################################

use Getopt::Std;

$opt_m = "";
$opt_a = "";
$opt_l = 0;
$opt_c = 0;

getopts('hlcm:a:t:');
$err = "";
if (!$opt_m) {
    $err .= "Must specify file containing summary of production merchant profile keys\n";
    $opt_h = 1;
}
if (!$opt_a) {
    $err .= "Must specify file containing authorization data\n";
    $opt_h = 1;
}
if ($opt_t) {
    $mKeyLen = $opt_t;
} else {
    $mKeyLen = $MAX_KEY_LEN;
}
if ($mKeyLen > $MAX_KEY_LEN) {
    $err .= "Merchant key must be no more than $MAX_KEY_LEN bytes.\n";
    $opt_h = 1;
}

################# yxc changed the title #########################

print "Merchant Key \t\t= $opt_m\nAuth of interest \t= $opt_a\nKey length chose\t= $mKeyLen\nWhether left justify \t= $opt_l\nWhether Cash/Merch \t= $opt_c\n\n";
if ($opt_h) {
    print STDERR $err;
    print STDERR $usage;
    print STDERR "Merchant Key \t\t= $opt_m\nAuth of interest \t= $opt_a\nKey length chose\t= $mKeyLen\nWhether left justify \t= $opt_l\nWhether Cash/Merch \t= $opt_c\n\n";
    exit (1);
}

###############################################
# 
# Read in the keys
#
###############################################
if ($opt_m =~ m/\.gz$/ || $opt_m =~ m/\.Z$/) {
    $MPROF = "gzip -dc $opt_m |";
} else {
    $MPROF = "< $opt_m";
}

#
# Read in the merchant profile keys.  Pay some attention to the default
# records (those with all-blank Merchant ID's), keeping track of those
# seperately.  Exit loop if a line is too short.  (Remember that
# length() includes the newline.)
#
$nIn = 0;
$nDef = 0;
open MPROF or die "Can't open merchant profile keys $MPROF";
while (defined($line = <MPROF>)) {
    $nIn++;
    if (length $line < $MAX_KEY_LEN + 1) {
        print STDERR "Bad MP line length of ", length $line, " --- exiting loop\n";
        last;
    }
    if (substr($line, 4, ($MAX_KEY_LEN - 4)) eq " " x ($MAX_KEY_LEN - 4)) {
        $nDef++;
    } else {
        # chomp $line;
        $key = substr($line, 0, $mKeyLen);
        $mKeys{$key}++;
    }
}
close MPROF;

#
# If we are truncating the merchant ID's, we need to look into how many
# are non-unique.  We still want to keep track of what may or may not
# get a hit from theses truncated keys, but we need to keep that
# separately from the full hits.
#
$nRemoved = 0;
$nCollisions = 0;
$nOrig = 0;
# open COL, "| gzip -c > collisions.$$.gz";
for $key (keys %mKeys) {
    $nOrig++;
    if ($mKeys{$key} > 1) {
        $nRemoved++;
        $nCollisions += $mKeys{$key};
        $collisions{$key} = 1;
        # print COL "$key\n";
        undef $mKeys{$key};
    }
}

#
# Let the use know what we've done!
#

################ yxc added the line and title ###################

print "============================================================\n"; 
print "Results below from mapping the mechant key:\n\n";
print "Read in $nIn profiles, $nDef defaults and ",
    scalar keys %mKeys, " profile keys\n";
print "Found $nOrig keys, with $nCollisions collisions.  Removed $nRemoved keys\n\n";



###############################################
#
# Read in the auths!
#
###############################################
if ($opt_a =~ m/\.gz$/ || $opt_a =~ m/\.Z$/) {
    $AUTHS = "gzip -dc $opt_a |";
} else {
    $AUTHS = "< $opt_a";
}

#
# Read in all of the auths.  Exit the loop on the first line that is too
# short.
#
# Read in the mcc and 16-byte mid.  Then take care of any justification
# issues.  Then seperate 15-byt mid from the "fill" byte.  Then try our
# match, keeping track of a number of possibilities.
#
$nFull = 0;
$nBMID = 0;
$nMCCD = 0;
$nGblD = 0;
$nColl = 0;
$nFill = 0;
$nIn = 0;
open AUTHS or die "Can't open merchant profile keys $AUTHS";
while (defined($line = <AUTHS>)) {
    $nIn++;
    if (length $line < 139) {
        print STDERR "Bad auths line length of ", length $line, " --- exiting loop\n";
        last;
    }

    ($type, $mcc, $midRaw) = unpack '@63a1 @84a4 @123a16', $line;

    next if ($opt_c && $type !~ /[CM]/o);

    if ($opt_l) {
        $midRaw =~ s/(\s+)(.*)/$2$1/o;
    }
    ($mid, $fil) = unpack 'a15a1', $midRaw;

    if ($fil ne " ") {
        $nFill++;
    }

    #
    # Look for how we match!
    #
    if ($mcc !~ /^\d\d\d\d$/o || $mcc eq "0000") {
        $nGblD++;
    } elsif ($mid =~ /^[ 0]*$/o) {
        $nBMID++;
    } elsif (defined($mKeys{substr("$mcc$mid", 0, $mKeyLen)})) {
        $nFull++;
        # print STDERR "|$mcc| |$mid| FULL $nIn\n";
    } elsif (defined($collisions{substr("$mcc$mid", 0, $mKeyLen)})) {
        $nColl++;
    } else {
        $nMCCD++;
        # print STDERR "|$mcc| |$mid| MCCD $nIn\n";
    }
}
close AUTHS;

$total = $nFull  + $nMCCD + $nBMID + $nColl + $nGblD;
$total = 1 if ($total == 0);

#
# Report it all
#
################ yxc modified the digit of number and title##################
# yxc changed the number format #
$nFull_per = sprintf ("%.2f", 100*$nFull/$total); 
$nMCCD_per = sprintf ("%.2f", 100*$nMCCD/$total);
$nColl_per = sprintf ("%.2f", 100*$nColl/$total);
$nBMID_per = sprintf ("%.2f", 100*$nBMID/$total);
$nGblD_per = sprintf ("%.2f", 100*$nGblD/$total);
$nFill_per = sprintf ("%.2f", 100*$nFill/$total);

printf "%10s \t\t %12s \t %8s\n", "Categories","Count","Percent";
print "------------------------------------------------------------\n";
printf "%15s \t %12d \t %6.2f%%\n", "Match a full key    ",$nFull, $nFull_per; 
printf "%24s %12d \t %6.2f%%\n", "Match >= a truncated Key",$nColl, $nColl_per;
printf "%20s \t %12d \t %6.2f%%\n", "Valid MID not found ",$nMCCD, $nMCCD_per;
printf "%20s \t %12d \t %6.2f%%\n", "Bad MID but good MCC",$nBMID, $nBMID_per;
printf "%20s \t %12d \t %6.2f%%\n", "Bad MCC (blank/zero)",$nGblD, $nGblD_per;
print "------------------------------------------------------------\n\n";

printf "%25s = %6d => %4.2f%%\n", "Number of non-blank in 16th position of MID",$nFill, $nFill_per;

#print "nFull = $nFull => ",$nFull_per , "\n";
#print "nMCCD = $nMCCD => ",$nMCCD_per,  "\n";
#print "nColl = $nColl => ",$nColl_per , "\n";
#print "nBMID = $nBMID => ",$nBMID_per , "\n";
#print "nGblD = $nGblD => ",$nGblD_per , "\n";
#print "\n";
#print "nFill = $nFill => ",$nFill_per , "\n";
#print "\n";
#print "skipped ", ($nIn - $total), " records.\n";

print "\nFinished on " . scalar localtime() . "\n\n";
exit (0);

################################################################################
# vim:cindent:
