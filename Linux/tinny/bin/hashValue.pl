#!/usr/local/bin/perl -w
#
#

# BEGIN { require ("/work/chewbacca/AMEX_MEX_RCP/scripts/hashValue.pm"); }
BEGIN { require ("/work/price/falcon/dvl/perl/hashValue.pm"); }

##############################

$hashType = $hashValue::sampstats;
$hashDirection = $hashValue::forward;

while (scalar (@ARGV) > 0) {
   $opt = shift (@ARGV);
   if ($opt eq "-t") {
      $hashType = $hashValue::testTrain;
   } elsif ($opt eq "-s") {
      $hashType = $hashValue::sampstats;
   } elsif ($opt eq "-b") {
      $hashDirection = $hashValue::backward;
   } else {
      print "Warning: unrecognized option $opt

    Usage:  $0 [-t | -s] [-b]

    Read from stdin a series of lines.  Each line begins with an account
    number in columns 1-19.

    Output the hash value.

    The value is based upon one of four hashing functions: by default
    the function used by sampstats to determine the sampling rate.

        -t  Calculate based upon the drv_falcon hash function for the
            TEST/TRAIN/VALIDATE split.

        -s  Calculate based upon the sampstats hash function for
            reducing the number of accouts in a sample (** default **).

        -b  Perform a backwards hash.  (Modifies the above algorithms.
            See dkg.)

";
      exit (1);
   }
}
      



##############################

while ($line = <STDIN>) {

   $acct=substr($line,0,19);
   $hashValue = &hashValue::hashValue ($acct,
                                       $hashType,
                                       $hashDirection),

   #############################################
   printf "%4d\n", $hashValue;
}
