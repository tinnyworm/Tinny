#!/usr/local/bin/perl -w
#
#
# Author: pcd (Paul C. Dulany)
# Date: June 15, 2003
#
$usage = 
"Usage: $0 [-c <split_col_spec>] [-s <output_start_col>] [-l <label>]

Splits the data based upon a given split column range.

   -c <col_spec>   Splits the input data into files based upon the colum
                   specification <col_spec>.  This is in the usual
                   format <start>-<stop>, i.e., -c 4-6 uses columns 4,
                   5, and 6.  The first character in a line is column 1.
                   The default is 1-1.

   -s <start_col>  Outputs the line starting at column <start_col>.
                   (This may be 1 to output the whole line.)  The
                   purpose is to allow the user to selectively remove
                   the split columns if they precede the data of
                   interest.  **NOTE: spaces will be replaced by
                   underscore characters!!** The first character in a
                   line is column 1.  The default is 1.

   -l <label>      Allows the designation of the output files.  They
                   will be named <label>.<splitField>.gz.  Please note
                   that any path in the label will be honored!!
                   Defaults to the executable name without the path.

Please be aware of the number of values in the split columns --- Perl
and the operating system have their limits...
";


################################################################################

$exename = $0;
$exename =~ s/\.pl$//o;
$exename =~ s/^.*\///o;

################################################################################
use Getopt::Std;

$output_start_col = 1;
$split_start_col = 1;
$split_length = 1;
$label = $exename;
$err = "";
getopts('hl:c:s:');

if ($opt_l) {
    $label = $opt_l;
}
if ($opt_c) {
    if ($opt_c =~ /(\d+)-(\d+)/o  || $opt_c =~ /(\d+)/o) {
        $split_start_col = $1;
        if (defined($2)) {
            $split_length = $2 - $1 + 1;
        } else {
            $split_length = 1;
        }
        if ($split_length < 1 || $split_start_col < 1) {
            $opt_h = 1;
            $err.="Bad split column specification\n";
        }
    } else {
        $opt_h = 1;
        $err.="Bad split column specification\n";
    }
}
if ($opt_s) {
    $output_start_col = $opt_s;
    if ($opt_s !~ m/^[0-9]+$/o) {
        $opt_h = 1;
        $err.="Bad output column specification\n";
    }
}
if ($opt_h) {
    print STDERR "$usage\n";
    print STDERR "$err\n";
    print STDERR "label:            $label\n";
    print STDERR "split_start_col:  $split_start_col\n";
    print STDERR "split_length:     $split_length\n";
    print STDERR "output_start_col: $output_start_col\n";
    exit 2;
}

$unpack_spec = '@' . ($split_start_col - 1) . "a$split_length" .
    ' @' . ($output_start_col - 1) . "a*";

################################################################################

while (defined($line = <STDIN>)) {
    ($splitField, $data) = unpack $unpack_spec, $line;
    $splitField =~ s/ /_/go;
    $fh = "f$splitField";
    if (!defined($fh{$splitField})) {
        open $fh, "| gzip -c | cat > $label.$splitField.gz";
        $fh{$splitField} = $fh;
    }
    print $fh $data;
}

# clean up
foreach $i (keys %fh) {
    close $fh{$i};
    $ret = $?;
    if ($ret != 0) {
        print STDERR "Warning: return code of $ret for pipe $i\n";
    }
}

################################################################################

# vim:cindent
