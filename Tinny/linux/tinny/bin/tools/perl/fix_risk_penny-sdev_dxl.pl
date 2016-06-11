#! /usr/local/bin/perl -w
# jas 7/2000
# Reads a tab file from STDIN, writes same to STDOUT. Checks risk entries and
# will set SDEV to 0 if less than 1.0

while(<>){
  if(/^(.+,\d+,\d+,\d+\.\d+,\d+\.\d+),(\d+\.\d\d),(\d+\.\d\d),(\d+\.\d+,\d+\.\d+),(\d+\.\d+),(\d+\.\d+)(,?\d?,?\d?)$/){
    $dol_legit_sdev = $2 * 1;
    $dol_fraud_sdev = $3 * 1;
    $dist_legit_sdev = $5 *1;
    $dist_fraud_sdev = $6 *1;
    if(($dol_legit_sdev >= 1.0 or $dol_legit_sdev == 0) and
       ($dol_fraud_sdev >= 1.0 or $dol_fraud_sdev == 0) and
       ($dist_legit_sdev >= 1.0 or $dist_legit_sdev == 0) and
       ($dist_fraud_sdev >= 1.0 or $dist_fraud_sdev == 0)){
      print $_;
    } else {
      $dol_legit_sdev = ($dol_legit_sdev >= 1.0)? $dol_legit_sdev:0.0;
      $dol_fraud_sdev = ($dol_fraud_sdev >= 1.0)? $dol_fraud_sdev:0.0;
      $dist_legit_sdev = ($dist_legit_sdev >= 1.0)? $dist_legit_sdev:0.0;
      $dist_fraud_sdev = ($dist_fraud_sdev >= 1.0)? $dist_fraud_sdev:0.0;
      $new = sprintf("%s,%4.2f,%4.2f,%s,%4.2f,%4.2f%s\n",
		     $1, $dol_legit_sdev, $dol_fraud_sdev,
		     $4, $dist_legit_sdev, $dist_fraud_sdev,
		     $7);
      print STDERR "Corrected line $.\n from\t$_ to\t$new";
      print $new;
    }
  } elsif(/^(.+,\d+,\d+,\d+\.\d+,\d+\.\d+),(\d+\.\d\d),(\d+\.\d\d)$/){
    $dol_legit_sdev = $2 * 1;
    $dol_fraud_sdev = $3 * 1;
    if(($dol_legit_sdev >= 1.0 or $dol_legit_sdev == 0) and
       ($dol_fraud_sdev >= 1.0 or $dol_fraud_sdev == 0)){
      print $_;
    } else {
      $dol_legit_sdev = ($dol_legit_sdev >= 1.0)? $dol_legit_sdev:0.0;
      $dol_fraud_sdev = ($dol_fraud_sdev >= 1.0)? $dol_fraud_sdev:0.0;
      $new = sprintf("%s,%4.2f,%4.2f\n",
		     $1, $dol_legit_sdev, $dol_fraud_sdev);
      print STDERR "Corrected line $.\n from\t$_ to\t$new";
      print $new;
    }
} else {
    print $_;
  }
}
