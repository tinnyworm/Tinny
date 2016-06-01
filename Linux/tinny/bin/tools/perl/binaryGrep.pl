#!/usr/local/bin/perl -w


$usage = "
Usage: $0 [-a | -e | -x] <reclen> <byte> <value>

Read stdin with records of length <reclen>.  Look at byte <byte>
(starting with 1 as the first byte of a record) for <value> in ascii
(-a), translatec to ebcdic (-e), or in hex.

Output records statisfying the request.

NOTE: -a and -e not working right now!! (Future expansion plans...)

";

use Getopt::Std;

$opt_a = $opt_e = $opt_x = 0;
getopts ('aexh');
if ($opt_a + $opt_e + $opt_x > 1) {
    print STDERR "Error: you can only specify one of -a, -e, or -x\n";
    $opt_h = 1;
} elsif ($opt_a + $opt_e + $opt_x == 0) {
    $opt_x = 1;
}

if ($opt_x != 1) {
    print STDERR "Error: Sorry, only -x is working right now.\n";
    $opt_h = 1;
}

$reclen = shift;
$byte = shift;
$value = shift;

if ($reclen < $byte) {
    print STDERR "Error: byte canot be greater than reclen!\n";
    $opt_h = 1;
}

if ($opt_x) {
    $findValue = hex ($value);
}
$unpackString = '@' . ($byte - 1) . "C";

if ($opt_h) {
    print STDERR $usage;
    print STDERR "  reclen = $reclen\n";
    print STDERR "  byte = $byte\n";
    print STDERR "  value = $value\n";
    print STDERR "  find-value = $findValue\n";
    print STDERR "  unpackString = $unpackString\n";
    exit 2;
}


$fileIndex = 0;
while (($numRead = read (STDIN, $line, $reclen)) > 0) {
    if ($numRead != $reclen) {
        print STDERR "Error on read at $fileIndex: tried for $reclen, got $numRead\n";
    }
    $fileIndex += $numRead;
    $byteFromLine = unpack $unpackString, $line;
    print $line if ($byteFromLine == $findValue);
}

# vim:cindent:
