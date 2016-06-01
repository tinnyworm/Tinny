#!/usr/local/bin/perl -w

$usage = "Usage: gzcat bigscr_file.gz | $0 <calibTable>\n" .
    "  Where <calibTable> is a calibration table (with or without\n" .
    "     rule language structure.\n" .
    "  Reads a big score file from stdin, and outputs to stdout.\n" .
    "\n" .
    "  **** ASSUMES that the table is in increments of 1. ****\n" .
    "\n";

if (scalar @ARGV != 1 || $ARGV[0] eq "-h" || $ARGV[0] eq "--help") {
    print $usage;
    exit 1;
}

$CALIB = $ARGV[0];
open CALIB or die "Can't open $CALIB";
$nLines = 0;
$nEntries = 0;
while (defined($line = <CALIB>)) {
    chomp $line;
    $nLines++;
    if ($line =~ m/^(\d+),(\d+)$/) {
        $nEntries++;
        $from = $1;
        $to = $2;
        $newScore[$from] = $to;
    }
}
close CALIB;
print STDERR "Calibration table has $nLines lines with $nEntries entries\n";
if ($nEntries != 1001) {
    print STDERR "Error!  Table must have 1001 entries [0,1000]\n", $usage;
    exit 1;
}


$nLines = 0;
while (defined ($line = <STDIN>)) {
    $oldScore = substr ($line, 83, 4);
    substr ($line, 83, 4) = sprintf "%4d", $newScore[$oldScore];
    print $line;
    $nLines++;
}
print STDERR "$nLines lines processed\n";

# vi:cindent:
