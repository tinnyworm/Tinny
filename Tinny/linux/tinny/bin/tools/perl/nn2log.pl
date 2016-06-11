#! /usr/local/bin/perl
# ---------------------------------------------------------------------------
#
# Input:
#        NNet recall file file, as obtained by 
#        $NNET -c $BASENAME.nnw  -o $BASENAME.nnlog -v $BASENAME.scl
#
# Output:
#        dmw log file
#
# Author: AXS, June 2000
# ---------------------------------------------------------------------------
while (<>) {
   ($index, $target, $nn_output)= split(/[ +,\t+]/);
    printf("%6d    %10.5f   %10.5f   %10.5f\n", 
	   $index, $target, $nn_output, abs($target-$nn_output));
}
