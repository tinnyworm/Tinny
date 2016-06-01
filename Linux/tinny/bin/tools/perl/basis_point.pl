#!/usr/local/bin/perl

# $Log: basis_point.pl,v $
# Revision 1.3  2006/01/30 22:45:53  dxl
# Add -t option for test percentage.
# Fixed the bug in amt_fraud_as_nonfraud calculation, (this amount should
# NOT be adjuested by sampling rate).
#
# Revision 1.2  2006/01/30 22:30:43  dxl
# Add -e to check the fraud type to be excluded, (the fraud transactions
# which won't be counted as fraud, but as non-fraud).
#
# Revision 1.1  2006/01/30 21:30:24  dxl
# Initial revision
#

use Getopt::Std;

$usage = "\n".
         "$0 calculate BASIS POINT, approved nonfraud, fraud and \n".
         "total SPENDING from a big score file.\n\n".
         "Usage: zcat <bigScr file> | $0 \n".
         "        [-s] <sampling_rate> (default 0.01) \n".
         "        [-t] <test percentage> (default 0.25) \n".
         "        [-e] <fraud type to be excluded> e.g. A or AM \n".
         "        [-h] help\n\n";

getopts("s:e:h");

if($opt_s) {
   $sample_rate = $opt_s;
}
else {
   $sample_rate = 0.01;
}

if($opt_t) {
   $test_perc = $opt_t;
} else {
   $test_perc = 0.25;
}

die $usage if($opt_h);

$amt_nonf=0;
$amt_f=0;
$amt_fasnonf=0;
while(<>) {
   ($amt,$ftag,$ftype,$score,$dec) = unpack("\@19 a14 \@59 a1 \@79 a1 \@83 a4 \@88 a1",$_);
   #print $amt," ",$ftag," ",$dec,"\n";
   if($dec eq "A" && $score >0 ) {
      if($ftag==0) {
         $amt_nonf += $amt;
      }
      elsif($ftag==1) {
         # excluded fraud type should be counted into fraud_as_non_fraud amt
         if($opt_e && $opt_e=~/$ftype/) {
            $amt_fasnonf += $amt;
         } else {
            $amt_f += $amt;
         }
      }
   }
}

$basis = 10000*$amt_f/($amt_nonf/$sample_rate+$amt_f+$amt_fasnonf);
$amt_total = ($amt_nonf/$sample_rate+$amt_f+$amt_fasnonf)/$test_perc;

printf("Basis Point:                %20.2f\n",$basis);
printf("Approved Nonfraud spending: %20.2f\n",$amt_nonf);
printf("Approved fraud spending:    %20.2f\n",$amt_f);
printf("Total Spending:             %20.2f\n\n",$amt_total);

if($opt_e) {
   printf("Note: fraud type $opt_e is excluded!\n\n");
}