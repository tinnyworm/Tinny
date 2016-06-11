#!/usr/local/bin/perl -w

use Getopt::Std;

$opt_c = 0;
$opt_p = 0;
$opt_n = 0;
$opt_h = 0;

$usage = "$0 [-c] [-p] [-n] [-h]\n" .
"\n"  .
"  -c => give cumulative count\n"  .
"  -p => give percentage\n"  .
"  -n => sort in numerical order\n"  .
"\n";

getopts('cpnh');
die $usage if ($opt_h);

$tot = 0;
while (<>) {
    chomp;
    $hist{$_}++;
    $tot++;
}

if ($opt_n || ($0 =~ m/histNum.pl$/)) {
    @keyList = sort {$a <=> $b} keys %hist;
} else {
    @keyList = sort keys %hist;
}

$cum=0;
foreach $i (@keyList) {
    printf "%12d", $hist{$i};
    if ($opt_c) {
        $cum += $hist{$i};
        printf "   %12d", $cum;
    }
    if ($opt_p) {
        $pct = 100 * $hist{$i} / $tot;
        printf "   %12.4f", $pct;
    }
    printf "   %s\n", $i;
}
