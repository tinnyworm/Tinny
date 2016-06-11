#!/usr/local/bin/perl -w
#

################################################################################

$usage = "$0 [-l] <file> [<label>] [<file2> [<label2>] ] ...\n" .
         "   -l => each file name is followed by a label\n";

################################################################################

use Getopt::Std;

getopts ('hl');

$useLabels = $opt_l;
if ($opt_l) {
    if (int((scalar @ARGV)/2) != ((scalar @ARGV)/2)) {
        print STDERR "If using -l, all files must be followed by a label\n";
        $opt_h = 1;
    }
}
if ($opt_h) {
    print STDERR $usage;
    exit (2);
}

################################################################################

while (scalar @ARGV) {
    push @files,  (shift @ARGV);
    push @labels, (shift @ARGV) if ($useLabels);
}
@labels = @files if (!$useLabels);

################################################################################

for ($i = 0; $i < scalar @files ; $i++) {
    open FILE, "<$files[$i]" or die "Can't read $files[$i]";
    while (defined($line = <FILE>)) {
        if ($line =~ m/# Table 1: all transactions, Basic stats/) {
            $line = <FILE>;  # Header
            $line = <FILE>;  # Header
            # ...
            while (defined($line = <FILE>) && $line !~ m/^\s*$/) {
                @F = split " ", $line;
                $l = $labels[$i];
                $pctFraud{$l}[$F[0]]      = $F[1];
                $pctNonraud{$l}[$F[0]]    = $F[2];
                $tfpr{$l}[$F[0]]          = $F[4];
                $pctFraudDol{$l}[$F[0]]   = $F[6];
                $pctNonraudDol{$l}[$F[0]] = $F[7];
                $dfpr{$l}[$F[0]]          = $F[9];
            }
            last;
        }
    }
    close FILE;
}

print ",";
for $lbl (@labels) {
    print ", $lbl,,,,,,";
}
print "\n\n";
print "score,";
for $lbl (@labels) {
    print ", \%fraud trans, \%nonfraud trans, tfpr, \%fraud dollars, \%nofraud dollars, dfpr,";
}
print "\n";
for ($scr = 1000; $scr >= 1; $scr--) {
    printf " %4d, ", $scr;
    for $lbl (@labels) {
        printf ", %8.4f, %8.4f, %8.3f, %8.4f, %8.4f, %8.3f, ",
        $pctFraud{$lbl}[$scr]      ,
        $pctNonraud{$lbl}[$scr]    ,
        $tfpr{$lbl}[$scr]          ,
        $pctFraudDol{$lbl}[$scr]   ,
        $pctNonraudDol{$lbl}[$scr] ,
        $dfpr{$lbl}[$scr]          ,
        ;
    }
    print "\n";
}

# vim:cindent:
