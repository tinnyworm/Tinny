#!/usr/local/bin/perl -w


$usage = "
Usage: $0 -r<reclen> -c<colSpec>

Read stdin with records of length <reclen>.  Write the columns <colSpec>
without delimiter (new recold length is the length of your col-spec).

Please note that we work in columns, not offset (think FORTRAN, not C ;-).

Limitations: Can only specify one range at this time.

";

use Getopt::Std;

getopts ('r:c:h');
$colSpec = $opt_c;
if ( $colSpec =~ /(\d+)-(\d+)/ ) {
    $startCol = $1;
    $length = $2 - $1 + 1;
    if ($length < 0 || $startCol < 0) {
        print STDERR "Bad column specification\n";
        $opt_h = 1;
    }
} else {
    print STDERR "Bad column specification\n";
    $opt_h = 1;
}


$reclen = $opt_r;

if ($reclen < $startCol + $length) {
    print STDERR "Error: colSpec canot reach beyond reclen!\n";
    $opt_h = 1;
}

if ($opt_h  || !$opt_r || !$opt_c ) {
    print STDERR $usage;
    print STDERR "  reclen = $reclen\n";
    print STDERR "  startCol = $startCol\n";
    print STDERR "  length = $length\n";
    exit 2;
}


$fileIndex = 0;
while (($numRead = read (STDIN, $line, $reclen)) > 0) {
    if ($numRead != $reclen) {
        print STDERR "Error on read at $fileIndex: tried for $reclen, got $numRead\n";
    }
    $fileIndex += $numRead;
    $numWritten = syswrite (STDOUT, $line, $length, $startCol - 1);
    if ($numWritten != $length) {
        print STDERR "Error on write at $fileIndex: tried for $length, got $numWritten\n";
    }
}

# vim:cindent:
