#!/usr/local/bin/perl -w
#
# Convert hex profile dump from falconr4 to a list of numbers.
#
# All values are floats except for the second and fourth variables
#
# Input line:
#
# ACCT_NUM_1 : VAR1 VAR2 VAR3 ... VAR153
# ACCT_NUM_2 : VAR1 VAR2 VAR3 ... VAR153
# ACCT_NUM_3 : VAR1 VAR2 VAR3 ... VAR153
#
# Output:
# 
# Mean, StdDev, min, max for VAR1...VAR153
#


$n = 0;
while (<>) {
    chomp;
    @F = split;
    # die "Number of fields in line is incorrect!" if (scalar @F != 224);
    $varnum = (scalar @F) - 2;
    # printf "%-19s ", $F[0];
    $n++;
    for ($i = 2; $i < $varnum + 2; $i++) {
        $j = $i - 1;
        $int = hex $F[$i];
        if ($j != 2 && $j != 4) {
            $tmp = pack 'I', $int;
            $float = unpack 'f', $tmp;
            $final = $float;
            # printf "%13g ", $float;
        } else {
            $final = $int;
            # printf "%13i ", $int;
        }

        $sum[$j] += $final;
        $sumsq[$j] += $final**2;
        if ($n == 1) {
            $min[$j] = $final;
            $max[$j] = $final;
        } else {
            $min[$j] = $final if ($final < $min[$j]);
            $max[$j] = $final if ($final > $max[$j]);
        }
    }
    # print "\n";
}

#
# WARNING: Breaking conventions for for loops and indicies: back to FORTRAN!
#
print "\n";
print "\n";
print "Number of vairables: $varnum\n";
print "Number of accounts: $n\n";
print " VARn  mean           Std.Dev.       min            max\n";
print " ----  -------------  -------------  -------------  -------------\n";
for ($i = 1; $i <= $varnum; $i++) {
    printf " %4i  %13g  %13g  %13g  %13g\n",
        $i,
        $sum[$i]/$n,
        sqrt(($sumsq[$i] - $sum[$i]**2/$n)/($n - 1)),
        $min[$i],
        $max[$i];

}
